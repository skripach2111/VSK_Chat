#include "notification.h"
//В зависимости от платформы подгружаем разные классы
#ifdef __ANDROID__
#include <QtAndroidExtras/QAndroidJniObject>
#else
#include <QSystemTrayIcon>
#endif

Notification::Notification(QObject *parent) : QObject(parent)
{
    connect(this, SIGNAL(notificationChanged()), this, SLOT(updateNotification()));
    m_notification = "";
// Если не андройд то создаём TrayIcon и наполняем его
#ifndef __ANDROID__
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(QIcon(":/chat80.png"));
    trayIcon->show();
#endif
}

void Notification::setNotification(QString notification)
{
    if (m_notification == notification)
        return;

    m_notification = notification;
    emit notificationChanged();
}

QString Notification::notification() const
{
    return m_notification;
}

//Две версии класса показа уведомлений - одну для Android другую для Desctiop
#ifdef __ANDROID__
void Notification::updateNotification()
{
    QAndroidJniObject javaNotification = QAndroidJniObject::fromString(m_notification);
//Обратите внимание на путь он нам сильно понадобиться далее!
    QAndroidJniObject::callStaticMethod<void>("org/chebfm/chat/NotificationClient",
                                       "notify",
                                       "(Ljava/lang/String;)V",
                                       javaNotification.object<jstring>());
}
#else
void NotificationClient::updateNotification()
{
    trayIcon->showMessage(QString("Test"),m_notification,QSystemTrayIcon::Information);
}
#endif
