#include "parserdt.h"

parserDT::parserDT(QObject *parent) :
    parser0(parent)
{
    m_header = false;
    m_nSet = 0;

    m_nFrame1st0 = 0xf02f4320;// 0xf07f2321;
    m_nFrame1st1 = 0x90f0ffff;// 0x4df0ff11;
    m_nID3200 = 0x19fff000;

}
bool parserDT::parse(myst_can *pCF)
{
    int ret=false;


    ret = checkHeader(pCF);
    //
    if(ret){// header set
        emit sigUdpLog(QByteArray(" datetime header set"));
        return ret;
    }
    else if(m_header){  // frame.data
        setDT(pCF);
    }

    return ret;
}

void parserDT::setDT(myst_can *pCF)
{
    char u8_1b=(char)0x1b;
    char u8_00=(char)0x0;
    unsigned int i0,i1,i2,i3,dt32;

    int id32dt = 0x19fff0ff;
    if(pCF->id32 != id32dt){
        return;
    }

    if(pCF->data[0] != u8_1b) return;
    if(pCF->data[1] != u8_00) return;

    i0=0x0ff & pCF->data[2];
    i1=0x0ff & pCF->data[3];
    i2=0x0ff & pCF->data[4];
    i3=0x0ff & pCF->data[5];

    dt32 = 0x0ff & i3;
    dt32 |= 0x0ff00 & ( i2<<8);
    dt32 |= 0x0ff0000 & (i1<<16);
    dt32 |= 0x0ff000000 & (i0<<24);

    //qDebug("dt32 : %x",dt32);

    if(m_nSet<1) cmdDT(dt32);



}
void parserDT::cmdDT(unsigned int dt32)
{
    QLocale e = QLocale::c();

    QDateTime dt(QDate(2018,1,1),QTime(0,0,0),Qt::UTC);
    QDateTime dt1;
    qint64 secs =0;
    secs += dt32;
    //QDateTime dt(QDate(2018,1,1),QTime(0,0,secs),Qt::UTC);
    //qDebug("   secs: %lld",secs);
    dt1=dt.addSecs(secs);
    //qDebug("   dt.add.secs: %s     s:%d",dt.toString("hh:mm:ss").toLatin1().data(),dt.time().second());
    //qDebug("   dt1.add.secs: %s",dt1.toString("hh:mm:ss").toLatin1().data());
    QByteArray cmd("/bin/date -u -s ");
    cmd.append(e.toString(dt1,"\"dd MMM yyyy hh:mm:ss\""));
    m_process.execute(cmd);
    //p0.start(cmd.data());
    //p1.start("echo a >> /home/root/qt/t.txt");

    m_nSet++;

    emit sigUdpLog(cmd);

    //qDebug("2018 -- date -u -s %s",e.toString( dt2018, "dd MMM yyyy hh:mm:ss").toLatin1().data());

}




