#ifndef CLIENT_H
#define CLIENT_H

#include <QDialog>
#include <QtNetwork/QTcpSocket>
#include <QTime>
#include <QMessageBox>
#include <QNetworkInterface>
#include "popup.h"

#include <QDebug>

enum COMMAND { AUTH = 1, ONLINE = 2, LISTEN = 3, DISC = 4, OK_DISK = 5, HISTORY_MESSAGE = 6, ERROR = 202};

namespace Ui {
class Client;
}

class Client : public QDialog
{
    Q_OBJECT

public:
    explicit Client(QWidget *parent = nullptr);
    ~Client();

private:
    Ui::Client *ui;
    quint16 nNextBlockSize;

    QString name;
    QTcpSocket *pSocket;
    QHostAddress host;
    short unsigned int port;

    PopUp *popupMessage;

private slots:
    void slotConnected();
    void slotDisconnected();
    void slotError(QAbstractSocket::SocketError);
    void slotReadyRead();
    void on_pB_connect_clicked();
    void on_pB_disconnect_clicked();
    void on_pB_send_clicked();
    void slotEnterPressed();
};

void GenerateMessage(int COMMAND, QByteArray &message, QByteArray &block);

#endif // CLIENT_H
