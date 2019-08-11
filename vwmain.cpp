#include "vwmain.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include <linux/can.h>
#include <linux/can/raw.h>



vwmain::vwmain(QObject *parent) :
    QObject(parent)
{
    m_bUdpLog = true;
    m_logAddr=QHostAddress("192.168.101.233");
    m_logPort = 57702;

    m_u8echo200 = 0;
    m_flagDebug = true;

    m_nCountCF = 0;

    initUDPcanDump();

    //testDTstring();

    QTimer::singleShot(5000,this,SLOT(slotReadFile200()));
    //cansend();

    slotUDPlog(QByteArray(" == app2018 ======== "));

    connect(&m_parseDT,SIGNAL(sigUdpLog(QByteArray)),this,SLOT(slotUDPlog(QByteArray)));
}

void vwmain::slotReadFile200()
{
    QFile file("/home/root/qt/cmd200");
    if(file.exists()){
        if(file.open(QIODevice::ReadOnly)){
        m_baFile200.clear();
        m_baFile200=file.readAll();
        file.close();
        }
    }
    else{
        QTimer::singleShot(2000,this,SLOT(slotReadFile200()));
    }

}

QString vwmain::getDTstring(unsigned secs)
{

}

void vwmain::testDTstring()
{
    // datetime toString
    QDateTime dt;
    QLocale e = QLocale::c();
    dt = QDateTime::currentDateTime();
    qDebug("date -s %s",e.toString( dt, "MMM yyyy hh:ss").toLatin1().data());

    QDateTime dt2018(QDate(2018,1,1),QTime(0,0),Qt::UTC);
    qDebug("2018 -- date -u -s %s",e.toString( dt2018, "dd MMM yyyy hh:mm:ss").toLatin1().data());

}
void vwmain::slotUDPlog(QByteArray ba)
{
    if(m_bUdpLog) m_pUDPcanDump->writeDatagram(ba,m_logAddr,m_logPort);

}

void vwmain::initUDPcanDump()
{
    m_pUDPlog = new QUdpSocket(this);

    m_pUDPcanDump = new QUdpSocket(this);
    //QHostAddress gAddr=QHostAddress("239.255.43.21");

    m_pUDPcanDump->bind(QHostAddress::Any,57700,QUdpSocket::ShareAddress);
    //m_pus->joinMulticastGroup(QHostAddress("239.255.43.21"));

    connect(m_pUDPcanDump, SIGNAL(readyRead()),this, SLOT(slotCANdump()));

}
void vwmain::slotCANdump()
{
    while (m_pUDPcanDump->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(m_pUDPcanDump->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;

        m_pUDPcanDump->readDatagram(datagram.data(), datagram.size(),&sender, &senderPort);

        if(datagram.size()==sizeof(struct myst_can)){
            m_qCANin.enqueue(datagram);
            QTimer::singleShot(0,this,SLOT(slotFrameParse()));
        }
    }

}
int vwmain::isValidCF(myst_can *pCF)
{
    int ret=0;
    int m = pCF->id32 & 0x0ffff00;

    if(m == 0x011f000) return 0x11;

    if(m == 0x0fff000) return 0x0ff;

    return ret;
}
bool vwmain::isValidQ7(myst_can *pCF)
{
    //if(m_flagDebug) qDebug(" func.isValidQ7 ................");

    bool ret=false;
    int *p32;

    if(!m_p7.m_bHeader){
        //if(m_flagDebug) qDebug(" func.isValidQ7 ............. no header id32:%x",pCF->id32);
        if(pCF->id32==0x1911f000){
            p32=(int*)pCF->data;
            //if(m_flagDebug) qDebug(" func.isValidQ7 ............. no header %x-%x",p32[0],p32[1]);
            if(p32[0]==(int)0xf01f4320 && p32[1]==(int)0x8df1ff11){
                m_p7.m_bHeader = true;
                ret = true;
            }
        }
    }
    else{// header received
        //if(m_flagDebug) qDebug(" func.isValidQ7 ............. header ");
        if(pCF->id32 == 0x1911f0ff){
            p32 = (int*)pCF->data;
            if(p32[0]==0x02ff023c && p32[1]==(int)0xeeeeeeee){
                m_p7.m_bHeader = false;
                ret = true;
                doEcho7();
            }
            else if(p32[0]==0x03ff033c && p32[1]==(int)0xeeeeeeee){
                m_p7.m_bHeader = false;
                ret = true;
                doEcho200();
            }

        }
    }

    return ret;

}

void vwmain::doEcho7()
{
    //if(m_flagDebug) qDebug(" func.doEcho7 ................");

    struct myst_can stcan;
    QByteArray ba,ba1;
    int *p32;
    p32=(int*)stcan.data;

    stcan.id32 = 0x19f01100;
    stcan.len = 8;
    p32[0]=0x111f4320;
    p32[1]=0x8df1fff0;
    ba.clear();
    ba.append((char*)(&stcan),sizeof(struct myst_can));
    emit sigCANsend(ba,0);

    stcan.id32 = 0x19f011ff;
    stcan.data[0]=0x3c;
    stcan.data[1]=0x41;
    stcan.data[2]=0;
    stcan.data[3]=0;
    stcan.data[4]=0;
    stcan.data[5]=m_nCountCF;
    stcan.data[6]=0x01;
    stcan.data[7]=0;

    ba1.clear();
    ba1.append((char*)(&stcan),sizeof(struct myst_can));
    emit sigCANsend(ba1,0);

}
void vwmain::doEcho200()
{
    bool flagRead=false;
    QByteArray ba200;
    int offset=0;
    int len;
    char ee=(char)0xee;
    int i;
    m_u8echo200++;
    //if(m_flagDebug) qDebug(" func.doEcho7 ................");

    struct myst_can stcan;
    QByteArray ba,ba1;
    int *p32;
    p32=(int*)stcan.data;

    len = m_baFile200.size();
    if(len<1){
        //QTimer::singleShot(1000,this,SLOT(slotReadFile200()));
        return;
    }
    else if(len<200){
        ba200.clear();
        ba200.append(m_baFile200.data(),len);
        m_baFile200.clear();
        flagRead = true;
        //QTimer::singleShot(2000,this,SLOT(slotReadFile200()));
    }
    else{// >=200
        ba200.clear();
        ba200.append(m_baFile200.data(),200);
        m_baFile200.remove(0,200);
        if(m_baFile200.size()<1) flagRead = true;
            //QTimer::singleShot(2000,this,SLOT(slotReadFile200()));

    }
    stcan.id32 = 0x19f01100;
    stcan.len = 8;
    p32[0]=0x111f6323;
    p32[1]=0x85f2fff0;
    ba.clear();
    ba.append((char*)(&stcan),sizeof(struct myst_can));
    emit sigCANsend(ba,0);

    offset=0;
    stcan.id32 = 0x19f01101;
    stcan.data[0]=0x3c;
    memcpy(stcan.data+1,ba200.data()+offset,7);
    offset += 7;

    ba1.clear();
    ba1.append((char*)(&stcan),sizeof(struct myst_can));
    emit sigCANsend(ba1,0);

    for(i=2;i<0x1a;i++){
        stcan.id32 = 0x19f01100 | (i & 0x0ff);
        memcpy(stcan.data,ba200.data()+offset,8);
        offset += 8;

        ba1.clear();
        ba1.append((char*)(&stcan),sizeof(struct myst_can));
        emit sigCANsend(ba1,0);

    }

    stcan.id32 = 0x19f011ff;
    memcpy(stcan.data,ba200.data()+offset,1);
    offset = 0;
    stcan.data[1]=ee;
    stcan.data[2]=ee;
    stcan.data[3]=ee;
    stcan.data[4]=ee;
    stcan.data[5]=ee;
    stcan.data[6]=ee;
    stcan.data[7]=ee;

    ba1.clear();
    ba1.append((char*)(&stcan),sizeof(struct myst_can));
    emit sigCANsend(ba1,0);

    if(flagRead){
        QFile file("/home/root/qt/cmd200");
        if(file.exists()){
            file.remove();
        }
        QTimer::singleShot(2000,this,SLOT(slotReadFile200()));

    }

#if 0
    stcan.id32 = 0x19f01100;
    stcan.len = 8;
    p32[0]=0x111f6323;
    p32[1]=0x85f2fff0;
    ba.clear();
    ba.append((char*)(&stcan),sizeof(struct myst_can));
    emit sigCANsend(ba,0);

    stcan.id32 = 0x19f01101;
    stcan.data[0]=0x3c;
    stcan.data[1]=m_u8echo200;
    stcan.data[2]=m_u8echo200;
    stcan.data[3]=m_u8echo200;
    stcan.data[4]=m_u8echo200;
    stcan.data[5]=m_u8echo200;
    stcan.data[6]=m_u8echo200;
    stcan.data[7]=m_u8echo200;

    ba1.clear();
    ba1.append((char*)(&stcan),sizeof(struct myst_can));
    emit sigCANsend(ba1,0);

    for(i=2;i<0x1a;i++){
        stcan.id32 = 0x19f01100 | (i & 0x0ff);
        stcan.data[0]=i;
        stcan.data[1]=i;
        stcan.data[2]=i;
        stcan.data[3]=i;
        stcan.data[4]=i;
        stcan.data[5]=i;
        stcan.data[6]=i;
        stcan.data[7]=i;

        ba1.clear();
        ba1.append((char*)(&stcan),sizeof(struct myst_can));
        emit sigCANsend(ba1,0);

    }

    stcan.id32 = 0x19f011ff;
    stcan.data[0]=m_u8echo200;
    stcan.data[1]=ee;
    stcan.data[2]=ee;
    stcan.data[3]=ee;
    stcan.data[4]=ee;
    stcan.data[5]=ee;
    stcan.data[6]=ee;
    stcan.data[7]=ee;

    ba1.clear();
    ba1.append((char*)(&stcan),sizeof(struct myst_can));
    emit sigCANsend(ba1,0);
#endif
}
bool vwmain::parseFile(myst_can *pCF)
{
    bool ret=m_parserFile.parse(pCF);
    return ret;
}
bool vwmain::parseFile46(myst_can *pCF)
{
    bool ret=m_parserFile46.parse(pCF);
    return ret;
}
bool vwmain::parseFile60(myst_can *pCF)
{
    bool ret=m_pkt60130.parse(pCF);
    return ret;
}
bool vwmain::parseDT(myst_can *pCF)
{
    bool ret=m_parseDT.parse(pCF);
    return ret;
}

void vwmain::slotFrameParse()
{
    int des;
    struct myst_can stcan;
    QByteArray ba;
    if(m_qCANin.isEmpty()) return;

    ba=m_qCANin.dequeue();
    memcpy(&stcan,ba.data(),sizeof(struct myst_can));
#if 0
    qDebug("udp.can id:%x len:%d %02x %02x %02x %02x %02x %02x %02x %02x",
           stcan.id32,stcan.len,
           0x0ff & stcan.data[0],
            0x0ff & stcan.data[1],
            0x0ff & stcan.data[2],
            0x0ff & stcan.data[3],
            0x0ff & stcan.data[4],
            0x0ff & stcan.data[5],
            0x0ff & stcan.data[6],
            0x0ff & stcan.data[7]);
#endif

    des = isValidCF(&stcan);
    switch(des){
    case 0x0ff:// broadcast
        parseDT(&stcan);
        parseFile60(&stcan);
        break;
    case 0x11:
        isValidQ7(&stcan);
        parseFile(&stcan);
        parseFile46(&stcan);
        m_nCountCF++;
        break;
    default:
        break;
    }

}




int vwmain::getCF(int id32, int len, char *p, struct canfd_frame *cf)
{
    int i;
    int ret = CAN_MTU;

    memset(cf, 0, sizeof(*cf)); /* init CAN FD frame, e.g. LEN = 0 */

    cf->can_id = id32;
    cf->can_id |= CAN_EFF_FLAG;   /* then it is an extended frame */

    cf->len = len;
    for(i=0;i<len;i++){
        cf->data[i]=p[i];
    }

    return ret;

}

int vwmain::cansend()
{
    int s; /* can raw socket */
    int required_mtu;
    //int mtu;
    //int enable_canfd = 1;
    struct sockaddr_can addr;
    struct canfd_frame frame;
    struct ifreq ifr;
    char p[8]={(char)0x11,(char)0x22,(char)0x33,(char)0x44,(char)0x55,(char)0x66,(char)0x77,(char)0x88};


    required_mtu = getCF(0x1f112233,8,p,&frame);
    /* open socket */
    if ((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
        qDebug("socket");
        return -1;
    }

    //strncpy(ifr.ifr_name, argv[1], IFNAMSIZ - 1);
    strcpy(ifr.ifr_name,"can0");
    ifr.ifr_name[IFNAMSIZ - 1] = '\0';
    ifr.ifr_ifindex = if_nametoindex(ifr.ifr_name);
    if (!ifr.ifr_ifindex) {
        qDebug("if_nametoindex");
        return 1;
    }

    memset(&addr, 0, sizeof(addr));
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    /* disable default receive filter on this RAW socket */
    /* This is obsolete as we do not read from the socket at all, but for */
    /* this reason we can remove the receive list in the Kernel to save a */
    /* little (really a very little!) CPU usage.                          */
    setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, NULL, 0);

    if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        return 1;
    }

    /* send frame */
    if (write(s, &frame, required_mtu) != required_mtu) {
        qDebug("write");
        return 1;
    }

    close(s);

    return 0;
}

void vwmain::testSend()
{
    QByteArray ba;
    struct myst_can stCAN;
    stCAN.id32 = 0x1f112233;
    stCAN.len = 8;
    for(int i=0;i<8;i++) stCAN.data[i]=i;

    ba.append((char*)(&stCAN),sizeof(struct myst_can));

    emit sigCANsend(ba,1);
}

