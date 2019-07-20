#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <QObject>
//тут я думаю всё понятно что и зачем? Нет? Тогда в комментарии...
#ifndef __ANDROID__
#include <QSystemTrayIcon>
#endif

class Notification : public QObject
{
    Q_OBJECT
public:
    explicit Notification(QObject *parent = 0);

    void setNotification(QString notification);
    QString notification() const;

signals:
    void notificationChanged();

private slots:
    void updateNotification();

private:
    QString m_notification;
#ifndef __ANDROID__
    QSystemTrayIcon *trayIcon;
#endif
};

#endif // NOTIFICATION_H
