#ifndef VWMAIN_H
#define VWMAIN_H

#include <QObject>
#include <QUdpSocket>
#include <QDateTime>
#include <QLocale>
#include <QQueue>
#include <QTimer>

#include "myst.h"
#include "parser7.h"

class vwmain : public QObject
{
    Q_OBJECT
public:
    explicit vwmain(QObject *parent = 0);
    bool m_flagDebug;

    int m_nCountCF;// valid CAN frame

    QQueue<QByteArray> m_qCANin;
    bool isValidCF(struct myst_can *pCF);
    bool isValidQ7(struct myst_can *pCF);
    void doEcho7();
    parser7 m_p7;

    QUdpSocket *m_pUDPcanDump;

    int cansend();// can-utils/cansend
    int getCF(int id32,int len,char *p, struct canfd_frame *cf);

    void initUDPcanDump();

    QString getDTstring(unsigned secs);// set system time cmdline





    void testDTstring();

signals:
    void sigCANsend(QByteArray ba,int qn);

public slots:
    void slotFrameParse();

    void slotCANdump();
    void testSend();


};

#endif // VWMAIN_H
