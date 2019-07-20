#include "server.h"
#include "ui_server.h"

Server::Server(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Server)
{
    ui->setupUi(this);

    nNextBlockSize = 0;
    nCountOnline = 0;
}

Server::~Server()
{
    delete ui;
}

void Server::on_pB_start_clicked()
{
    ui->lW_logs->addItem("Starting server...");
    ui->lW_logs->scrollToBottom();

    pServer = new QTcpServer(this);
    timer = new QTimer();
    timerOnline = new QTimer();

    connect(timer, SIGNAL(timeout()), this, SLOT(slotUpdateTimer()));
    connect(timerOnline, SIGNAL(timeout()), this, SLOT(slotPing()));

    timeStart = QTime::currentTime();
    timer->start(1000);
    timerOnline->start(50);

    host = ui->lE_host->text();
    port = ui->lE_port->text().toInt();

    if (!pServer->listen(host, port)) {
        QMessageBox::critical(0,
                              "Server Error",
                              "Unable to start the server:"
                              + pServer->errorString()
                              );
        pServer->close();
        return;
    }
    connect(pServer, SIGNAL(newConnection()),
            this,         SLOT(slotNewConnection())
            );

    ui->gB_settings->setEnabled(false);
    ui->gB_logs->setEnabled(true);
    ui->gB_state->setEnabled(true);

    ui->lW_logs->addItem("Server local address: " + host.toString() + ":" + QVariant(port).toString());
    ui->lW_logs->scrollToBottom();
}

void Server::slotNewConnection()
{
    ui->lW_logs->addItem("Connecting client...");
    ui->lW_logs->scrollToBottom();

    Client tmp_client;
    tmp_client.pSocket = pServer->nextPendingConnection();

    connect(tmp_client.pSocket, SIGNAL(disconnected()), tmp_client.pSocket, SLOT(deleteLater()));
    connect(tmp_client.pSocket, SIGNAL(readyRead()), this, SLOT(slotReadClient()));

    listClients.push_back(tmp_client);

    doSendToClientHystoryMessage(tmp_client.pSocket);
}

void Server::slotReadClient()
{
    QTcpSocket* pClientSocket = (QTcpSocket*)sender();
    QDataStream in(pClientSocket);
    in.setVersion(QDataStream::Qt_5_13);
    for (;;) {
        if (!nNextBlockSize) {
            if (pClientSocket->bytesAvailable() < sizeof(quint16)) {
                break;
            }

            in >> nNextBlockSize;
        }

        if (pClientSocket->bytesAvailable() < nNextBlockSize) {
            break;
        }

        int COMMAND;
        in >> COMMAND;

        switch (COMMAND)
        {
        case AUTH:
        {
            QString name;
            in >> name;

            listClients[listClients.size()-1].name = name;
            listClientsName.append(name);
            doSendToClientsOnline();

            ui->lW_logs->addItem("Client connected! Name: " + name);
            ui->lW_logs->scrollToBottom();

            nCountOnline++;
            ui->l_online->setText("Online: " + QVariant(nCountOnline).toString());

            break;
        }
        case LISTEN:
        {
            QString message;
            in >> message;

            message = QTime::currentTime().toString() + " " + message;
            listMessage.push_back(message);
            doSendToClientsMessage(message);

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

void Server::doSendToClientsMessage(const QString &message)
{
    QByteArray  arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_13);
    out << (quint16)0 << LISTEN << message;

    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));
    out.device()->seek(arrBlock.size()-1);

    for(int i = 0; i < listClients.size(); i++)
    {
        if(listClients[i].pSocket->state() == QTcpSocket::ConnectedState)
            listClients[i].pSocket->write(arrBlock);
        else
        {
            doSendToClientDisconnect(listClients[i].pSocket);

            ui->lW_logs->addItem(listClients[i].name + " disconnected...");
            ui->lW_logs->scrollToBottom();

            listClients.erase(listClients.begin()+i);
            listClientsName.erase(listClientsName.begin()+i);

            nCountOnline--;
            ui->l_online->setText("Online: " + QVariant(nCountOnline).toString());
        }
    }
}

void Server::doSendToClientsOnline()
{
    QByteArray  arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_13);
    out << (quint16)0 << ONLINE << listClientsName;

    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));
    out.device()->seek(arrBlock.size()-1);

    for(int i = 0; i < listClients.size(); i++)
    {
        if(listClients[i].pSocket->state() == QTcpSocket::ConnectedState)
            listClients[i].pSocket->write(arrBlock);
    }
}

void Server::on_pB_stop_clicked()
{
    timer->stop();

    ui->gB_settings->setEnabled(true);
    ui->gB_logs->setEnabled(false);
    ui->gB_state->setEnabled(false);

    ui->l_online->setText("Online: 0");
    ui->l_time->setText("00:00:00");
    ui->lW_logs->clear();

    nCountOnline = 0;

    ui->lW_logs->addItem("Server local address: " + host.toString() + ":" + QVariant(port).toString());
    ui->lW_logs->scrollToBottom();

    for(int i = 0; i < listClients.size(); i++)
        listClients[i].pSocket->disconnected();

    listClients.clear();
    listClientsName.clear();
    pServer->disconnect();

    delete pServer;
    delete timer;
}

QTime operator - ( const QTime & t1, const QTime & t2 )
{
    int diff = t2.msecsTo(t1);
    return QTime(diff / HOUR_MS,
                 (diff % HOUR_MS) / MIN_MS,
                 (diff % MIN_MS) / SEC_MS,
                 diff % SEC_MS);
}

void Server::slotUpdateTimer()
{
    QTime tmp = QTime::currentTime();
    tmp = tmp - timeStart;

    ui->l_time->setText(tmp.toString());

    if(listMessage.size() >= 1024)
        listMessage.erase(listMessage.begin(), listMessage.begin() + 512);
}

void Server::doSendToClientDisconnect(QTcpSocket *pSocket)
{
    if(pSocket->state() == QTcpSocket::ConnectedState)
    {
        QByteArray  arrBlock;
        QDataStream out(&arrBlock, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_5_13);
        out << (quint16)0 << OK_DISK;

        out.device()->seek(0);
        out << quint16(arrBlock.size() - sizeof(quint16));
        out.device()->seek(arrBlock.size()-1);

        pSocket->write(arrBlock);
    }
}

void Server::doSendToClientHystoryMessage(QTcpSocket *pSocket)
{
    if(pSocket->state() == QTcpSocket::ConnectedState)
    {
        QByteArray  arrBlock;
        QDataStream out(&arrBlock, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_5_13);
        out << (quint16)0 << HISTORY_MESSAGE << listMessage;

//        for(int i = 1; i < listMessage.size(); i++)
//            out << listMessage[i].size() << listMessage[i];

        out.device()->seek(0);
        out << quint16(arrBlock.size() - sizeof(quint16));
        out.device()->seek(arrBlock.size()-1);

        pSocket->write(arrBlock);
    }
}

void Server::DisconnectClient(int number)
{
    listClients.erase(listClients.begin()+number);
    ui->lW_logs->addItem(listClientsName[number] + " disconnected...");
    listClientsName.erase(listClientsName.begin()+number);

    ui->lW_logs->scrollToBottom();

    nCountOnline--;
    ui->l_online->setText("Online: " + QVariant(nCountOnline).toString());
}

void Server::slotPing()
{
    for(int i = 0; i < listClients.size(); i++)
        if(listClients[i].pSocket->state() == QTcpSocket::UnconnectedState || listClients[i].pSocket->state() == QTcpSocket::ClosingState)
        {
            DisconnectClient(i);

            doSendToClientsOnline();
        }
}
