#ifndef VWMAIN_H
#define VWMAIN_H

#include <QObject>
#include <QUdpSocket>

#include "myst.h"

class vwmain : public QObject
{
    Q_OBJECT
public:
    explicit vwmain(QObject *parent = 0);

    QUdpSocket *m_pUDPcanDump;

    int cansend();// can-utils/cansend
    int getCF(int id32,int len,char *p, struct canfd_frame *cf);

    void initUDPcanDump();





signals:
    void sigCANsend(QByteArray ba,int qn);

public slots:
    void slotCANdump();
    void testSend();


};

#endif // VWMAIN_H
