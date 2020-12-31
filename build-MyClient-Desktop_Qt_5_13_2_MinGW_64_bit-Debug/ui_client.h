/********************************************************************************
** Form generated from reading UI file 'client.ui'
**
** Created by: Qt User Interface Compiler version 5.13.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CLIENT_H
#define UI_CLIENT_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>

QT_BEGIN_NAMESPACE

class Ui_Client
{
public:
    QGridLayout *gridLayout;
    QGroupBox *gB_connection;
    QGridLayout *gridLayout_2;
    QLabel *label;
    QLabel *label_3;
    QLabel *label_2;
    QLineEdit *lE_host;
    QSpinBox *sB_port;
    QLineEdit *lE_name;
    QPushButton *pB_connect;
    QGroupBox *gB_online;
    QGridLayout *gridLayout_3;
    QListWidget *lW_online;
    QPushButton *pB_disconnect;
    QGroupBox *gB_chat;
    QGridLayout *gridLayout_4;
    QLineEdit *lE_message;
    QPushButton *pB_send;
    QListWidget *lW_chat;

    void setupUi(QDialog *Client)
    {
        if (Client->objectName().isEmpty())
            Client->setObjectName(QString::fromUtf8("Client"));
        Client->resize(668, 563);
        gridLayout = new QGridLayout(Client);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gB_connection = new QGroupBox(Client);
        gB_connection->setObjectName(QString::fromUtf8("gB_connection"));
        gridLayout_2 = new QGridLayout(gB_connection);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        label = new QLabel(gB_connection);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout_2->addWidget(label, 0, 0, 1, 1);

        label_3 = new QLabel(gB_connection);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        gridLayout_2->addWidget(label_3, 0, 2, 1, 1);

        label_2 = new QLabel(gB_connection);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout_2->addWidget(label_2, 0, 1, 1, 1);

        lE_host = new QLineEdit(gB_connection);
        lE_host->setObjectName(QString::fromUtf8("lE_host"));

        gridLayout_2->addWidget(lE_host, 1, 0, 1, 1);

        sB_port = new QSpinBox(gB_connection);
        sB_port->setObjectName(QString::fromUtf8("sB_port"));
        sB_port->setMaximum(6000);
        sB_port->setValue(2111);

        gridLayout_2->addWidget(sB_port, 1, 1, 1, 1);

        lE_name = new QLineEdit(gB_connection);
        lE_name->setObjectName(QString::fromUtf8("lE_name"));

        gridLayout_2->addWidget(lE_name, 1, 2, 1, 1);

        pB_connect = new QPushButton(gB_connection);
        pB_connect->setObjectName(QString::fromUtf8("pB_connect"));

        gridLayout_2->addWidget(pB_connect, 2, 0, 1, 3);


        gridLayout->addWidget(gB_connection, 1, 0, 1, 3);

        gB_online = new QGroupBox(Client);
        gB_online->setObjectName(QString::fromUtf8("gB_online"));
        gB_online->setEnabled(false);
        gB_online->setMinimumSize(QSize(160, 0));
        gB_online->setMaximumSize(QSize(160, 16777215));
        gridLayout_3 = new QGridLayout(gB_online);
        gridLayout_3->setSpacing(6);
        gridLayout_3->setContentsMargins(11, 11, 11, 11);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        lW_online = new QListWidget(gB_online);
        lW_online->setObjectName(QString::fromUtf8("lW_online"));

        gridLayout_3->addWidget(lW_online, 0, 0, 1, 1);


        gridLayout->addWidget(gB_online, 2, 0, 1, 1);

        pB_disconnect = new QPushButton(Client);
        pB_disconnect->setObjectName(QString::fromUtf8("pB_disconnect"));
        pB_disconnect->setEnabled(false);

        gridLayout->addWidget(pB_disconnect, 3, 0, 1, 1);

        gB_chat = new QGroupBox(Client);
        gB_chat->setObjectName(QString::fromUtf8("gB_chat"));
        gB_chat->setEnabled(false);
        gridLayout_4 = new QGridLayout(gB_chat);
        gridLayout_4->setSpacing(6);
        gridLayout_4->setContentsMargins(11, 11, 11, 11);
        gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
        lE_message = new QLineEdit(gB_chat);
        lE_message->setObjectName(QString::fromUtf8("lE_message"));

        gridLayout_4->addWidget(lE_message, 1, 0, 1, 1);

        pB_send = new QPushButton(gB_chat);
        pB_send->setObjectName(QString::fromUtf8("pB_send"));

        gridLayout_4->addWidget(pB_send, 1, 1, 1, 1);

        lW_chat = new QListWidget(gB_chat);
        lW_chat->setObjectName(QString::fromUtf8("lW_chat"));
        lW_chat->setMinimumSize(QSize(460, 360));

        gridLayout_4->addWidget(lW_chat, 0, 0, 1, 2);


        gridLayout->addWidget(gB_chat, 2, 1, 2, 2);


        retranslateUi(Client);

        QMetaObject::connectSlotsByName(Client);
    } // setupUi

    void retranslateUi(QDialog *Client)
    {
        Client->setWindowTitle(QCoreApplication::translate("Client", "Client", nullptr));
        gB_connection->setTitle(QCoreApplication::translate("Client", "Connection:", nullptr));
        label->setText(QCoreApplication::translate("Client", "Host:", nullptr));
        label_3->setText(QCoreApplication::translate("Client", "Name:", nullptr));
        label_2->setText(QCoreApplication::translate("Client", "Port:", nullptr));
        lE_host->setText(QCoreApplication::translate("Client", "77.109.49.147", nullptr));
        pB_connect->setText(QCoreApplication::translate("Client", "Connect", nullptr));
        gB_online->setTitle(QCoreApplication::translate("Client", "Online:", nullptr));
        pB_disconnect->setText(QCoreApplication::translate("Client", "Disconnect", nullptr));
        gB_chat->setTitle(QCoreApplication::translate("Client", "Chat:", nullptr));
        pB_send->setText(QCoreApplication::translate("Client", "Send", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Client: public Ui_Client {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CLIENT_H
