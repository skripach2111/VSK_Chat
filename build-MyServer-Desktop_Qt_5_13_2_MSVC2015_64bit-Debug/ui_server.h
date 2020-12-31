/********************************************************************************
** Form generated from reading UI file 'server.ui'
**
** Created by: Qt User Interface Compiler version 5.13.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SERVER_H
#define UI_SERVER_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_Server
{
public:
    QGridLayout *gridLayout;
    QGroupBox *gB_state;
    QVBoxLayout *verticalLayout_2;
    QLabel *l_online;
    QLabel *label_5;
    QLabel *l_time;
    QPushButton *pB_stop;
    QGroupBox *gB_settings;
    QVBoxLayout *verticalLayout;
    QLabel *label;
    QLineEdit *lE_host;
    QLabel *label_2;
    QLineEdit *lE_port;
    QPushButton *pB_start;
    QGroupBox *gB_logs;
    QGridLayout *gridLayout_2;
    QListWidget *lW_logs;

    void setupUi(QDialog *Server)
    {
        if (Server->objectName().isEmpty())
            Server->setObjectName(QString::fromUtf8("Server"));
        Server->resize(360, 440);
        Server->setMinimumSize(QSize(360, 440));
        gridLayout = new QGridLayout(Server);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gB_state = new QGroupBox(Server);
        gB_state->setObjectName(QString::fromUtf8("gB_state"));
        gB_state->setEnabled(false);
        verticalLayout_2 = new QVBoxLayout(gB_state);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        l_online = new QLabel(gB_state);
        l_online->setObjectName(QString::fromUtf8("l_online"));

        verticalLayout_2->addWidget(l_online);

        label_5 = new QLabel(gB_state);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        verticalLayout_2->addWidget(label_5);

        l_time = new QLabel(gB_state);
        l_time->setObjectName(QString::fromUtf8("l_time"));

        verticalLayout_2->addWidget(l_time);

        pB_stop = new QPushButton(gB_state);
        pB_stop->setObjectName(QString::fromUtf8("pB_stop"));

        verticalLayout_2->addWidget(pB_stop);


        gridLayout->addWidget(gB_state, 0, 1, 1, 1);

        gB_settings = new QGroupBox(Server);
        gB_settings->setObjectName(QString::fromUtf8("gB_settings"));
        verticalLayout = new QVBoxLayout(gB_settings);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        label = new QLabel(gB_settings);
        label->setObjectName(QString::fromUtf8("label"));

        verticalLayout->addWidget(label);

        lE_host = new QLineEdit(gB_settings);
        lE_host->setObjectName(QString::fromUtf8("lE_host"));

        verticalLayout->addWidget(lE_host);

        label_2 = new QLabel(gB_settings);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        verticalLayout->addWidget(label_2);

        lE_port = new QLineEdit(gB_settings);
        lE_port->setObjectName(QString::fromUtf8("lE_port"));

        verticalLayout->addWidget(lE_port);

        pB_start = new QPushButton(gB_settings);
        pB_start->setObjectName(QString::fromUtf8("pB_start"));

        verticalLayout->addWidget(pB_start);


        gridLayout->addWidget(gB_settings, 0, 0, 1, 1);

        gB_logs = new QGroupBox(Server);
        gB_logs->setObjectName(QString::fromUtf8("gB_logs"));
        gB_logs->setEnabled(false);
        gridLayout_2 = new QGridLayout(gB_logs);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        lW_logs = new QListWidget(gB_logs);
        lW_logs->setObjectName(QString::fromUtf8("lW_logs"));

        gridLayout_2->addWidget(lW_logs, 0, 0, 1, 1);


        gridLayout->addWidget(gB_logs, 1, 0, 1, 2);


        retranslateUi(Server);

        QMetaObject::connectSlotsByName(Server);
    } // setupUi

    void retranslateUi(QDialog *Server)
    {
        Server->setWindowTitle(QCoreApplication::translate("Server", "Server", nullptr));
        gB_state->setTitle(QCoreApplication::translate("Server", "State:", nullptr));
        l_online->setText(QCoreApplication::translate("Server", "Online: 0", nullptr));
        label_5->setText(QCoreApplication::translate("Server", "Working:", nullptr));
        l_time->setText(QCoreApplication::translate("Server", "00:00:00", nullptr));
        pB_stop->setText(QCoreApplication::translate("Server", "Stop", nullptr));
        gB_settings->setTitle(QCoreApplication::translate("Server", "Settings:", nullptr));
        label->setText(QCoreApplication::translate("Server", "Host:", nullptr));
        label_2->setText(QCoreApplication::translate("Server", "Port:", nullptr));
        pB_start->setText(QCoreApplication::translate("Server", "Start", nullptr));
        gB_logs->setTitle(QCoreApplication::translate("Server", "Logs:", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Server: public Ui_Server {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SERVER_H
