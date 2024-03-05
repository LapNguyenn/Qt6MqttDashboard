
#include "widget.h"

#include <QApplication>
#include <QScreen>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.setWindowTitle("Mqtt Client Setting");
    QList<QScreen*> screens = QGuiApplication::screens();
    QRect screenGeometry = screens.at(0)->geometry();
    w.setGeometry(screenGeometry);
    w.show();
    return a.exec();
}
