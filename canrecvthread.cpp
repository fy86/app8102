#include "canrecvthread.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include <linux/can.h>
#include <linux/can/raw.h>

canrecvthread::canrecvthread(QObject *parent) :
    QThread(parent)
{
}

void canrecvthread::run()
{
    QUdpSocket udp;

    struct ifreq ifr;
    struct sockaddr_can addr;
    struct can_frame frame;
    int s;
    int ret;
    struct myst_can CANdata;
    QByteArray ba;
    char buf[200];
    int i;

    memset(&ifr, 0x0, sizeof(ifr));
    memset(&addr, 0x0, sizeof(addr));
    memset(&frame, 0x0, sizeof(frame));
    /* open CAN_RAW socket */
    s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    /* convert interface sting "can0" into interface index */
    strcpy(ifr.ifr_name, "can0");
    ioctl(s, SIOCGIFINDEX, &ifr);
    /* setup address for bind */
    addr.can_ifindex = ifr.ifr_ifindex;
    addr.can_family = PF_CAN;
    /* bind socket to the can0 interface */
    bind(s, (struct sockaddr *)&addr, sizeof(addr));
    for(;;){
        //qDebug(" thread can recv .............. thread can recv");
        ret=read(s, &frame, sizeof(frame));
        //qDebug(" thread can recv .............. thread can recv  ret");
        if(ret>0){
            if(frame.can_dlc<=8){

                sprintf(buf," thread can recv .............. thread can recv  len:%d",frame.can_dlc);
                ba.clear();
                ba.append(buf);
                //emit sigUdpLog(ba);

                CANdata.len = frame.can_dlc;
                CANdata.id32 = 0x1fffffff & frame.can_id;
                for(i=0;i<CANdata.len;i++) CANdata.data[i]=frame.data[i];
                udp.writeDatagram((char*)(&CANdata),sizeof(struct myst_can),QHostAddress::LocalHost,57700);
            }
        }
    }
#if 0
    /* first fill, then send the CAN frame */
    frame.can_id = 0x23;
    strcpy((char *)frame.data, "hello");
    frame.can_dlc = 5;
    write(s, &frame, sizeof(frame));
    /* first fill, then send the CAN frame */
    frame.can_id = 0x23;
    strcpy((char *)frame.data, "iCC2012");
    frame.can_dlc = 7;
    write(s, &frame, sizeof(frame));
#endif
    close(s);

}
