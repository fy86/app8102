#include "cansendthread.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include <linux/can.h>
#include <linux/can/raw.h>

cansendthread::cansendthread(QObject *parent) :
    QThread(parent)
{
    m_bOpen = false;// not open
}

void cansendthread::run()
{
    if(!m_bOpen) return;// openCAN first

    for(;;){
        if(!m_qBA0.isEmpty()){
            canSend();
        }
        else if(!m_qBA1.isEmpty()){
            canSend();
        }
        m_sem.acquire();
    }

}
void cansendthread::slotQadd(QByteArray ba, int qn)
{
    if(qn==0){
        m_qBA0.enqueue(ba);
    }
    else{
        m_qBA1.enqueue(ba);
    }
    m_sem.release();
}

int cansendthread::getCF(canfd_frame *cf)
{
    int i;
    int ret = CAN_MTU;
    struct myst_can CANdata;
    QByteArray ba;

    if(!m_qBA0.isEmpty()){
        ba=m_qBA0.dequeue();
    }
    else if(!m_qBA1.isEmpty()){
        ba=m_qBA1.dequeue();
    }
    else {
        return 0;
    }
    memcpy(&CANdata,ba.data(),sizeof(struct myst_can));

    memset(cf, 0, sizeof(*cf)); /* init CAN FD frame, e.g. LEN = 0 */

    cf->can_id = CANdata.id32;
    cf->can_id |= CAN_EFF_FLAG;   /* then it is an extended frame */

    cf->len = CANdata.len;

    for(i=0;i<cf->len;i++){
        cf->data[i]=CANdata.data[i];
    }

    return ret;
}
int cansendthread::canSend()
{
    int required_mtu=CAN_MTU;
    struct canfd_frame frame;

    required_mtu = getCF(&frame);
    if(required_mtu > 0) write(m_sock, &frame, required_mtu);

    return 0;
}

int cansendthread::openCAN(char *pCAName)
{
    //int s; /* can raw socket */
    //int required_mtu=CAN_MTU;
    //int mtu;
    //int enable_canfd = 1;
    struct sockaddr_can addr;
    //struct canfd_frame frame;
    struct ifreq ifr;
    //char p[8]={(char)0x11,(char)0x22,(char)0x33,(char)0x44,(char)0x55,(char)0x66,(char)0x77,(char)0x88};

    m_bOpen = false;

    //required_mtu = getCF(0x1f112233,8,p,&frame);
    /* open socket */
    if ((m_sock = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
        qDebug("socket");
        return -1;
    }

    //strncpy(ifr.ifr_name, argv[1], IFNAMSIZ - 1);
    strcpy(ifr.ifr_name,pCAName);
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
    setsockopt(m_sock, SOL_CAN_RAW, CAN_RAW_FILTER, NULL, 0);

    if (bind(m_sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        return 1;
    }
#if 0
    /* send frame */
    if (write(m_sock, &frame, required_mtu) != required_mtu) {
        qDebug("write");
        return 1;
    }

    //close(m_sock);
#endif

    m_bOpen = true;
    return 0;
}



