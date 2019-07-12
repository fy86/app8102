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
    initUDPcanDump();

    //cansend();
}

void vwmain::initUDPcanDump()
{
    m_pUDPcanDump = new QUdpSocket(this);
    //QHostAddress gAddr=QHostAddress("239.255.43.21");

    m_pUDPcanDump->bind(QHostAddress::Any,57700,QUdpSocket::ShareAddress);
    //m_pus->joinMulticastGroup(QHostAddress("239.255.43.21"));

    connect(m_pUDPcanDump, SIGNAL(readyRead()),this, SLOT(slotCANdump()));

}
void vwmain::slotCANdump()
{
    struct myst_can CANdata;

    while (m_pUDPcanDump->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(m_pUDPcanDump->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;

        m_pUDPcanDump->readDatagram(datagram.data(), datagram.size(),&sender, &senderPort);

        if(datagram.size()==sizeof(struct myst_can)){
            memcpy(&CANdata,datagram.data(),sizeof(struct myst_can));
            qDebug("udp.can id:%x len:%d %02x %02x %02x %02x %02x %02x %02x %02x",
                   CANdata.id32,CANdata.len,
                   0x0ff & CANdata.data[0],
                    0x0ff & CANdata.data[1],
                    0x0ff & CANdata.data[2],
                    0x0ff & CANdata.data[3],
                    0x0ff & CANdata.data[4],
                    0x0ff & CANdata.data[5],
                    0x0ff & CANdata.data[6],
                    0x0ff & CANdata.data[7]);
        }
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

