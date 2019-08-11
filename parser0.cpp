#include "parser0.h"

parser0::parser0(QObject *parent) :
    QObject(parent)
{
    m_flagDebug = true;

    m_header = false;
}

bool parser0::checkHeader(myst_can *pCF)
{
    bool ret=false;
    int *p32;

    //qDebug(" func.checkHeader() ................");
    p32 = (int*)(pCF->data);

    if(pCF->id32 == m_nID3200
            && p32[0]==m_nFrame1st0
            && p32[1]==m_nFrame1st1 ){
        m_header = true;
        ret = true;
        emit sigUdpLog(QByteArray(" checkHeader:  parser header found"));
    }

    return ret;
}

bool parser0::parse(myst_can *pCF)
{
    return false;
}

void parser0::printBA(QByteArray ba)
{
    int len=ba.size();
    int i;
    char buf[10];
    QByteArray pba;
    sprintf(buf,"ba.len:%d - ",len);
    pba.append(buf);
    for(i=0;i<len;i++){
        sprintf(buf,"%02X ",0x0ff & ba.at(i));
        pba.append(buf);
    }
    if(m_flagDebug) qDebug("%s",pba.data());

}

