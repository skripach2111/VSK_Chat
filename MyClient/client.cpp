#include "client.h"
#include "ui_client.h"

Client::Client(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Client)
{
    ui->setupUi(this);

    connect(ui->lE_message, SIGNAL(returnPressed()), this, SLOT(slotEnterPressed()));

    popupMessage = new PopUp();

    nNextBlockSize = 0;
}

Client::~Client()
{
    delete popupMessage;
    delete ui;
}

void Client::on_pB_connect_clicked()
{
    pSocket = new QTcpSocket();
    nNextBlockSize = 0;

    connect(pSocket, SIGNAL(readyRead()), this, SLOT(slotReadyRead()));
    connect(pSocket, SIGNAL(connected()), this, SLOT(slotConnected()));
    connect(pSocket, SIGNAL(disconnected()), this, SLOT(slotDisconnected()));
    connect(pSocket, SIGNAL(error(QAbstractSocket::SocketError)),this, SLOT(slotError(QAbstractSocket::SocketError)));

    host = ui->lE_host->text();
    port = ui->sB_port->value();
    name = ui->lE_name->text();

    pSocket->connectToHost(host, port);
}

void Client::slotConnected()
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    out << (quint16)0 << AUTH << name;

    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));

    pSocket->write(block);

    ui->gB_connection->setEnabled(false);
    ui->gB_online->setEnabled(true);
    ui->gB_chat->setEnabled(true);
    ui->pB_disconnect->setEnabled(true);
}

void Client::slotReadyRead()
{
    QDataStream in(pSocket);
    in.setVersion(QDataStream::Qt_5_13);
    for (;;)
    {
        if (!nNextBlockSize)
        {
            if (pSocket->bytesAvailable() < sizeof(quint16))
            {
                break;
            }

            in >> nNextBlockSize;
        }

        if (pSocket->bytesAvailable() < nNextBlockSize)
        {
            break;
        }

        int COMMAND;

        in >> COMMAND;

        switch (COMMAND)
        {
        case LISTEN:
        {
            QString message;

            in >> message;

            ui->lW_chat->addItem(message);
            ui->lW_chat->scrollToBottom();

            if(!this->isActiveWindow())
            {
                QApplication::alert(this);
                popupMessage->setPopupText(message);
                popupMessage->show();
            }

            break;
        }
        case ONLINE:
        {
            QStringList listUsers;

            in >> listUsers;

            ui->lW_online->clear();
            ui->lW_online->addItems(listUsers);

            break;
        }
        case HISTORY_MESSAGE:
        {
            QStringList tmp;
            in >> tmp;

            ui->lW_chat->addItems(tmp);

            ui->lW_chat->scrollToBottom();

            break;
        }
        default:
        {
            break;
        }
        }

        nNextBlockSize = 0;
    }
}

void Client::slotError(QAbstractSocket::SocketError err)
{
    QString strError =
            "Error: " + (err == QAbstractSocket::HostNotFoundError ?
                             "The host was not found." :
                             err == QAbstractSocket::RemoteHostClosedError ?
                                 "The remote host is closed." :
                                 err == QAbstractSocket::ConnectionRefusedError ?
                                     "The connection was refused." :
                                     QString(pSocket->errorString())
                                     );

    QMessageBox msgBox;
    msgBox.setText("Error!");
    msgBox.setInformativeText(strError);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.setDefaultButton(QMessageBox::Ok);
    msgBox.exec();
}

void Client::slotDisconnected()
{
    ui->gB_connection->setEnabled(true);
    ui->gB_online->setEnabled(false);
    ui->gB_chat->setEnabled(false);
    ui->pB_disconnect->setEnabled(false);

    ui->lW_chat->clear();
    ui->lW_online->clear();
    ui->lE_message->clear();



    pSocket->disconnectFromHost();
}

void Client::on_pB_disconnect_clicked()
{
    slotDisconnected();
}


void Client::on_pB_send_clicked()
{
    if(ui->lE_message->text().size() != 0)
    {
        QString message;
        message = name + ": " + ui->lE_message->text();

        for(int i = 49; i < message.size(); i += 50)
            message.insert(i, "\n");

        QByteArray block;
        QDataStream out(&block, QIODevice::WriteOnly);

        message = name + ": " + ui->lE_message->text();

        out << (quint16)0 << LISTEN << message;

        out.device()->seek(0);

        out << (quint16)(block.size() - sizeof(quint16));
        pSocket->write(block);

        ui->lE_message->setText("");
    }
}

void Client::slotEnterPressed()
{
    on_pB_send_clicked();
}
