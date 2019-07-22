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
    QTcpSocket* pClientSocket = (QTcpSocket*)sender();
    QDataStream in(pClientSocket);

    in.setVersion(QDataStream::Qt_5_13);
    while(true)
    {
        if (nNextBlockSize == 0) {
            if (pClientSocket->bytesAvailable() < (int)sizeof(quint16))
                break;
            else
                in >> nNextBlockSize;
        }

        if (pClientSocket->bytesAvailable() < nNextBlockSize)
            break;
//        else{
//            nNextBlockSize = 0;
//            break;
//        }

    int messenger;
    in >> messenger;

    if(messenger%2 == 0)
    {
        int COMMAND;
        in >> COMMAND;;

        if(COMMAND == AUTH || COMMAND == LISTEN)
        {
            in >> messenger;

            if(messenger%2 != 0)
            {
                unsigned int size_message;
                in >> size_message;

                QByteArray encrypted_message;

                in >> encrypted_message;
                in >> messenger;

                if(messenger%2 == 0)
                {
                    for(int i = 0; i < (int)(encrypted_message.size()/2); i += 2)
                    {
                            char byte;
                            byte = encrypted_message[i];
                            encrypted_message[i] = encrypted_message[encrypted_message.size()-1-i];
                            encrypted_message[encrypted_message.size()-1-i] = byte;
                    }

                    for(int i = encrypted_message.size()-2; i >= 0; i--)
                        encrypted_message[i] = encrypted_message[i] - encrypted_message[i+1];

                    QDataStream input_message(&encrypted_message, QIODevice::ReadOnly);
                    QString message;
                    input_message >> message;

                    switch (COMMAND)
                    {
                    case AUTH:
                    {
                        bool correct = true;

                        for(int i = 0; i < listClientsName.size(); i++)
                            if(message == listClientsName[i])
                                doSendError(listClients[listClients.size()-1].pSocket, "Пользователь с таким именем уже есть в чате.");

                        if(correct)
                        {
                            listClients[listClients.size()-1].name = message;
                            listClientsName.append(message);
                            doSendToClientsOnline();

                            ui->lW_logs->addItem("Client connected! Name: " + listClientsName[listClientsName.size()-1]);
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
                        QString buff;
                        buff = QTime::currentTime().toString() + " ";

                        for(int i = 0; i < listClients.size(); i++)
                            if(pClientSocket == listClients[i].pSocket)
                            {
                                buff = buff + listClients[i].name + ": " + message;
                                i = listClients.size();
                            }

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
}

void Server::doSendToClientsMessage(const QString &message)
{
    ui->lW_logs->addItem("Send message");
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
    ui->lW_logs->addItem("Send clients online");
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
    ui->lW_logs->addItem("Send to client disconnected");
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
    ui->lW_logs->addItem("Send hystory message");
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
    ui->lW_logs->addItem("Disconnect clinet");
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
        }
}

void Server::GenerateMessage(int COMMAND, QByteArray &message, QByteArray &block)
{
    QDataStream out(&block, QIODevice::WriteOnly);
    int messenger;

    do
    {
        messenger = 0 + rand() % 32767;
    }
    while(messenger%2 != 0);

    out << (quint16)0 << messenger << COMMAND;

    do
    {
        messenger = 0 + rand() % 32767;
    }
    while(messenger%2 == 0);

    out << messenger << message.size();

    do
    {
        messenger = 0 + rand() % 32767;
    }
    while(messenger%2 != 0);

    for(int i = 0; i < message.size()-1; i++)
        message[i] = message[i] + message[i+1];

    for(int i = 0; i < (int)(message.size()/2); i += 2)
    {
            char byte;
            byte = message[i];
            message[i] = message[message.size()-1-i];
            message[message.size()-1-i] = byte;
    }

    out << message << messenger;

    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));
}

void Server::doSendError(QTcpSocket *pSocket, QString message)
{
    ui->lW_logs->addItem("Send error");
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
