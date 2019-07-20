#include "client.h"
#include <QApplication>
#include <QFlags>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Client w;
    w.setWindowFlag(Qt::WindowType::WindowMinimizeButtonHint);
    w.setWindowFlag(Qt::WindowType::WindowMaximizeButtonHint);
    w.setWindowFlag(Qt::WindowType::WindowContextHelpButtonHint, false);
    w.show();

    return a.exec();
}
