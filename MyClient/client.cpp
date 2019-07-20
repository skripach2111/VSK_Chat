#include "client.h"
#include "ui_client.h"

Client::Client(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Client)
{
    ui->setupUi(this);

    connect(ui->lE_message, SIGNAL(returnPressed()), this, SLOT(slotEnterPressed()));

    nNextBlockSize = 0;

    popupMessage = new PopUp();
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
    QByteArray message;
    QDataStream out(&message, QIODevice::WriteOnly);

    out << name;

    GenerateMessage(1, message, block);

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

        int messenger;
        in >> messenger;

        if(messenger%2 == 0)
        {
            int COMMAND;
            in >> COMMAND;

            if(COMMAND == AUTH || COMMAND == LISTEN || COMMAND == ONLINE || COMMAND == HISTORY_MESSAGE)
            {
                in >> messenger;

                if(messenger%2 != 0)
                {
                    unsigned int size_message;
                    in >> size_message;

                    char *message = new char [size_message];
                    //QByteArray message;

                    in.readBytes(message, size_message);

                    in >> messenger;

                    if(messenger%2 == 0)
                    {
                        for(int i = 0; i < size_message; i++)
                            message[i] = message[i]*messenger-size_message;

                        switch (COMMAND)
                        {
                        case LISTEN:
                        {
                            ui->lW_chat->addItem(message);
                            ui->lW_chat->scrollToBottom();

                            if(!this->isActiveWindow())
                            {
                                popupMessage->setPopupText(message);
                                popupMessage->show();
                                QApplication::alert(this);
                            }

                            break;
                        }
                        case ONLINE:
                        {
                            QByteArray mes;
                            QDataStream out(&mes, QIODevice::ReadWrite);

                            out << message;

                            QStringList listUsers;
                            out >> listUsers;

                            in >> listUsers;

                            ui->lW_online->clear();
                            ui->lW_online->addItems(listUsers);

                            break;
                        }
                            //        case OK_DISK:
                            //        {
                            //            pSocket->disconnected();
                            //            delete pSocket;


                            //            break;
                            //        }
                        case HISTORY_MESSAGE:
                        {
                            QByteArray mes;
                            QDataStream out(&mes, QIODevice::ReadWrite);

                            out << message;

                            QStringList tmp;
                            out >> tmp;

                            ui->lW_chat->addItems(tmp);

                            ui->lW_chat->scrollToBottom();

                            break;
                        }
                        case ERROR:
                        {
                            QMessageBox msgBox;
                            msgBox.setText("Error!");
                            msgBox.setInformativeText(message);
                            msgBox.setStandardButtons(QMessageBox::Ok);
                            msgBox.setIcon(QMessageBox::Critical);
                            msgBox.setDefaultButton(QMessageBox::Ok);
                            msgBox.exec();

                        break;
                        }
                        default:
                        {
                            break;
                        }
                        }
                    }
                }
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
    //    QByteArray block;
    //    QDataStream out(&block, QIODevice::WriteOnly);

    //    out << (quint16)0 << DISC << name;

    //    out.device()->seek(0);
    //    out << (quint16)(block.size() - sizeof(quint16));

    //    pSocket->write(block);

    slotDisconnected();
}


void Client::on_pB_send_clicked()
{
    if(ui->lE_message->text().size() != 0)
    {
        QByteArray message;
        QDataStream out(&message, QIODevice::WriteOnly);
        out << name << ": " << ui->lE_message->text();

        //        for(int i = 49; i < message.size(); i += 50)
        //            message.insert(i, "\n");

        QByteArray block;

        GenerateMessage(LISTEN, message, block);
        pSocket->write(block);

        ui->lE_message->setText("");
    }
}

void Client::slotEnterPressed()
{
    on_pB_send_clicked();
}

void GenerateMessage(int COMMAND, QByteArray &message, QByteArray &block)
{
    QDataStream out(&block, QIODevice::WriteOnly);
    int messenger;

    do
    {
        messenger = 0 + rand() % 32767;
    }
    while(messenger%2 == 0);

    out << (quint16)0 << messenger << COMMAND;

    do
    {
        messenger = 0 + rand() % 32767;
    }
    while(messenger%2 != 0);

    out << messenger << message.size();

    do
    {
        messenger = 0 + rand() % 32767;
    }
    while(messenger%2 == 0);

    for(int i = 0; i < message.size(); i++)
        message[i] = (message[i] + message.size())/messenger;

    out << message << messenger;

    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));
}
