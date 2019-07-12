#include <QCoreApplication>
#include <QTimer>

#include "vwmain.h"
#include "cansendthread.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    vwmain vw;
    cansendthread sthread;

    QObject::connect(&vw,SIGNAL(sigCANsend(QByteArray,int)),&sthread,SLOT(slotQadd(QByteArray,int)));

    sthread.openCAN("can0");
    sthread.start();

    qDebug("hello");

    vw.testSend();

    QTimer::singleShot(2000,&vw,SLOT(testSend()));

    return a.exec();
}
