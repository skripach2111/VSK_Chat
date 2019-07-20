#include "client.h"
#include <QApplication>
#include <QDesktopWidget>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Client w;

    QRect rectScreen;
    rectScreen = QApplication::desktop()->availableGeometry(&w);

    w.setGeometry(rectScreen);

    w.show();

    return a.exec();
}
