#include <QCoreApplication>
#include <QTimer>

#include "vwmain.h"
#include "cansendthread.h"
#include "canrecvthread.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    vwmain vw;
    cansendthread sthread;
    canrecvthread rthread;

    char pCAName[]="can0";

    QObject::connect(&vw,SIGNAL(sigCANsend(QByteArray,int)),&sthread,SLOT(slotQadd(QByteArray,int)));

    rthread.start();

    sthread.openCAN(pCAName);
    sthread.start();

    qDebug("v0.12 7.13");

    vw.testSend();

    QTimer::singleShot(2000,&vw,SLOT(testSend()));
    QTimer::singleShot(3000,&vw,SLOT(testSend()));

    return a.exec();
}

// v0.12 can.recv.id.mask 0x1f.ff.ff.ff
// v0.11 can.recv.test
// v0.10 can.send.thread.done



