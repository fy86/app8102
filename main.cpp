#include <QCoreApplication>

#include "vwmain.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    vwmain vw;

    qDebug("hello");

    return a.exec();
}
