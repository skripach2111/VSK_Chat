#ifndef SERVER_H
#define SERVER_H

#include <QDialog>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <QTime>
#include <QTimer>
#include <QMessageBox>
#include <vector>

#include <QDebug>

#define SEC_MS	1000
#define MIN_MS	(SEC_MS * 60)
#define HOUR_MS	(MIN_MS * 60)

using namespace std;

enum COMMAND { AUTH = 1, ONLINE = 2, LISTEN = 3, DISC = 4, OK_DISK = 5, HISTORY_MESSAGE = 6, ERROR = 202};

struct Client
{
    QString name;
    QTcpSocket *pSocket;
};

namespace Ui {
class Server;
}

class Server : public QDialog
{
    Q_OBJECT

    QTcpServer *pServer;
    quint16 nNextBlockSize;
    QHostAddress host;
    short unsigned int port;

    QStringList listClientsName;
    QStringList listMessage;
    vector <Client> listClients;
    vector <Client> listTmpClients;
    int nCountOnline;
    QTime timeStart;
    QTimer *timer;
    QTimer *timerOnline;

public:
    explicit Server(QWidget *parent = nullptr);
    ~Server();

    void doSendToClientsMessage(const QString& message);
    void doSendToClientsOnline();
    void doSendToClientDisconnect(QTcpSocket *pSocket);
    void doSendToClientHystoryMessage(QTcpSocket *pSocket);
    void DisconnectClient(QTcpSocket *pSocket);
    void doSendError(QTcpSocket *pSocket, QString message);
    void GenerateMessage(int COMMAND, QByteArray &message, QByteArray &block);

private:
    Ui::Server *ui;

private slots:
    void slotNewConnection();
    void slotReadClient();
    void on_pB_start_clicked();
    void on_pB_stop_clicked();
    void slotUpdateTimer();
    void slotPing();
};

#endif // SERVER_H
