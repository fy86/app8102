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
#include "pkt60130.h"
#include "parserfile.h"
#include "parserfile46.h"
#include "parserdt.h"

class vwmain : public QObject
{
    Q_OBJECT
public:
    explicit vwmain(QObject *parent = 0);
    bool m_flagDebug;

    QByteArray m_baFile200;

    parserDT m_parseDT;
    bool parseDT(struct myst_can *pCF);

    pkt60130 m_pkt60130;
    parserfile m_parserFile;
    parserfile46 m_parserFile46;
    bool parseFile(struct myst_can *pCF);
    bool parseFile60(struct myst_can *pCF);
    bool parseFile46(struct myst_can *pCF);

    int m_nCountCF;// valid CAN frame

    QQueue<QByteArray> m_qCANin;

    // ret(des)  0xff:boroadcast , 0x11:   other
    int isValidCF(struct myst_can *pCF);

    bool isValidQ7(struct myst_can *pCF);
    void doEcho7();
    void doEcho200();
    char m_u8echo200;

    parser7 m_p7;

    QUdpSocket *m_pUDPcanDump;

    bool m_bUdpLog;
    QHostAddress m_logAddr;
    short m_logPort;
    QUdpSocket *m_pUDPlog;

    int cansend();// can-utils/cansend
    int getCF(int id32,int len,char *p, struct canfd_frame *cf);

    void initUDPcanDump();

    QString getDTstring(unsigned secs);// set system time cmdline





    void testDTstring();

signals:
    void sigCANsend(QByteArray ba,int qn);

public slots:
    void slotUDPlog(QByteArray ba);
    void slotFrameParse();

    void slotCANdump();
    void testSend();

    void slotReadFile200();


};

#endif // VWMAIN_H
