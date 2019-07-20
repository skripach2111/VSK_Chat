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

    //doSendToClientHystoryMessage(tmp_client.pSocket);
}

void Server::slotReadClient()
{
    ui->lW_logs->addItem("Start read client...");
    ui->lW_logs->scrollToBottom();
    QTcpSocket* pClientSocket = (QTcpSocket*)sender();
    QDataStream in(pClientSocket);
    QByteArray qbarray;
    QDataStream out2(&qbarray, QIODevice::ReadWrite);
    in.setVersion(QDataStream::Qt_5_13);
    for (;;) {
        if (!nNextBlockSize) {
            if (pClientSocket->bytesAvailable() < sizeof(quint16)) {
                return;
            }

            in >> nNextBlockSize;
        }

        if (pClientSocket->bytesAvailable()+sizeof(quint16) < nNextBlockSize) {
            in >> qbarray;
        }
        else
            break;
    }


        ui->lW_logs->addItem("Read messendger...");
        ui->lW_logs->scrollToBottom();

        int messenger;
        out2 >> messenger;

        if(messenger%2 == 0)
        {
            ui->lW_logs->addItem("Read command...");
            ui->lW_logs->scrollToBottom();

            int COMMAND;
            out2 >> COMMAND;

            ui->lW_logs->addItem("Command: " + QVariant(COMMAND).toString());
            ui->lW_logs->scrollToBottom();

            if(COMMAND == 1 || COMMAND == 3)
            {
                ui->lW_logs->addItem("Read messendger...");
                ui->lW_logs->scrollToBottom();
                out2 >> messenger;

                if(messenger%2 != 0)
                {
                    ui->lW_logs->addItem("Read size...");
                    ui->lW_logs->scrollToBottom();
                    unsigned int size_message;
                    out2 >> size_message;

                    char *message = new char [size_message];
                    //QByteArray message;

                    ui->lW_logs->addItem("Read message...");
                    ui->lW_logs->scrollToBottom();

                    out2.readBytes(message, size_message);

                    QByteArray mes;
                    QDataStream out(&mes, QIODevice::ReadWrite);

                    out << message;

                    ui->lW_logs->addItem("Read messendger...");
                    ui->lW_logs->scrollToBottom();
                    out2 >> messenger;

                    if(messenger%2 == 0)
                    {
                        ui->lW_logs->addItem("Read message...");
                        ui->lW_logs->scrollToBottom();
                        for(int i = 0; i < size_message; i++)
                            mes[i] = mes[i]*messenger-size_message;

                        switch (COMMAND)
                        {
                        case AUTH:
                        {

                            ui->lW_logs->addItem("Start AUTH...");
                            ui->lW_logs->scrollToBottom();
                            bool correct = true;

                            for(int i = 0; i < listClientsName.size(); i++)
                                if(message == listClientsName[i])
                                    doSendError(listClients[listClients.size()-1].pSocket, "Пользователь с таким именем уже есть в чате.");

                            if(correct)
                            {
                                listClients[listClients.size()-1].name = mes;
                                listClientsName.append(mes);
                                doSendToClientsOnline();

                                ui->lW_logs->addItem("Client connected! Name: " + QVariant(mes).toString());
                                ui->lW_logs->scrollToBottom();

                                nCountOnline++;
                                ui->l_online->setText("Online: " + QVariant(nCountOnline).toString());

                                doSendToClientHystoryMessage(listClients[listClients.size()-1].pSocket);
                            }
                            else
                                DisconnectClient(pClientSocket);

                            break;
                        }
                        case LISTEN:
                        {
                            QString buff = mes;
                            buff = QTime::currentTime().toString() + " " + buff;
                            listMessage.push_back(buff);
                            doSendToClientsMessage(buff);

                            break;
                        }
                        default:
                        {
                            break;
                        }
                        }
                    }
                    else
                        DisconnectClient(pClientSocket);
                }
                else
                    DisconnectClient(pClientSocket);
            }
            else
                DisconnectClient(pClientSocket);
        }
        else
            DisconnectClient(pClientSocket);

        nNextBlockSize = 0;

}

void Server::doSendToClientsMessage(const QString &message)
{
    ui->lW_logs->addItem("doSendToClientMessage");
    ui->lW_logs->scrollToBottom();
    QByteArray block;
    QByteArray mes;
    QDataStream out(&mes, QIODevice::WriteOnly);

    out << message;

    GenerateMessage(LISTEN, mes, block);

    for(int i = 0; i < listClients.size(); i++)
    {
        if(listClients[i].pSocket->state() == QTcpSocket::ConnectedState)
            listClients[i].pSocket->write(block);
    }
}

void Server::doSendToClientsOnline()
{
    ui->lW_logs->addItem("doSendToClientOnline");
    ui->lW_logs->scrollToBottom();

    QByteArray block;
    QByteArray mes;
    QDataStream out(&mes, QIODevice::WriteOnly);

    out << listClientsName;

    GenerateMessage(ONLINE, mes, block);

    for(int i = 0; i < listClients.size(); i++)
    {
        if(listClients[i].pSocket->state() == QTcpSocket::ConnectedState)
            listClients[i].pSocket->write(block);
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
    ui->lW_logs->addItem("doSendToClientDisconnect");
    ui->lW_logs->scrollToBottom();
    if(pSocket->state() == QTcpSocket::ConnectedState)
    {
        QByteArray block;
        QByteArray mes;

        GenerateMessage(OK_DISK, mes, block);

        pSocket->write(block);
    }
}

void Server::doSendToClientHystoryMessage(QTcpSocket *pSocket)
{
    ui->lW_logs->addItem("doSendToClientHystoryMessage");
    ui->lW_logs->scrollToBottom();

    if(pSocket->state() == QTcpSocket::ConnectedState)
    {
        QByteArray block;
        QByteArray mes;
        QDataStream out(&mes, QIODevice::WriteOnly);

        out << listMessage;

        GenerateMessage(HISTORY_MESSAGE, mes, block);

        pSocket->write(block);
    }
}

void Server::DisconnectClient(QTcpSocket *pSocket)
{
    ui->lW_logs->addItem("DisconnectClinet");
    ui->lW_logs->scrollToBottom();

    for(int i = 0; i < listClients.size(); i++)
    {
        if(listClients[i].pSocket == pSocket)
        {
            ui->lW_logs->addItem(listClients[i].name + " disconnected...");
            listClients[i].pSocket->disconnected();
            listClients.erase(listClients.begin()+i);
            if(i < listClientsName.size())
            {
                listClientsName.erase(listClientsName.begin()+i);
                nCountOnline--;
                ui->l_online->setText("Online: " + QVariant(nCountOnline).toString());
            }

            i = listClients.size();
        }
    }

    ui->lW_logs->scrollToBottom();
}

void Server::slotPing()
{    
    for(int i = 0; i < listClients.size(); i++)
        if(listClients[i].name.size() != 0)
        {
            if(listClients[i].pSocket->state() == QTcpSocket::UnconnectedState || listClients[i].pSocket->state() == QTcpSocket::ClosingState)
            {
                DisconnectClient(listClients[i].pSocket);

                doSendToClientsOnline();
            }
            ui->lW_logs->addItem("ping");
            ui->lW_logs->scrollToBottom();
        }
}

void Server::GenerateMessage(int COMMAND, QByteArray &message, QByteArray &block)
{
    QDataStream out(&block, QIODevice::WriteOnly);
    int messenger;

    do
    {
        messenger = -32768 + rand() % 32767;
    }
    while(messenger%2 == 0);

    out << (quint16)0 << messenger << COMMAND;

    do
    {
        messenger = -32768 + rand() % 32767;
    }
    while(messenger%2 != 0);

    out << messenger << message.size();

    do
    {
        messenger = -32768 + rand() % 32767;
    }
    while(messenger%2 == 0);

    for(int i = 0; i < message.size(); i++)
        message[i] = (message[i] + message.size())/messenger;

    out << message << messenger;

    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));
}

void Server::doSendError(QTcpSocket *pSocket, QString message)
{
    ui->lW_logs->addItem("dosenderror...");
    ui->lW_logs->scrollToBottom();

    if(pSocket->state() == QTcpSocket::ConnectedState)
    {
        QByteArray block;
        QByteArray mes;
        QDataStream out(&mes, QIODevice::WriteOnly);

        out << message;

        GenerateMessage(ERROR, mes, block);

        pSocket->write(block);
    }
}
