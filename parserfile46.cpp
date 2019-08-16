#include "parserfile46.h"

parserfile46::parserfile46(QObject *parent) :
    parserfile(parent)
{
    m_const57 = 46;

    m_nFrame1st0 = 0xf07fe320;// 0xf07f2321;
    m_nFrame1st1 = 0x8ef0ff11;// 0x4df0ff11;
    m_nID3200 = 0x1911f000;
}

bool parserfile46::mk57(myst_can *pCF)
{
    char u8_3C=(char)0x3c;
    char u8_55=(char)0x55;
    bool ret = false;
    int len;

    int id24 = 0x1911f000;
    int m24 = 0xffffff00;
    int m=pCF->id32 & m24;
    int sn;
    if(m!=id24){
        return ret;
    }
    sn = 0x0ff & pCF->id32;

    switch(sn){
    case 1:
        if(pCF->data[0]!=u8_3C){
            ret = false;
        }
        else{
            ret = true;
            m_ba.clear();// init redo
            m_ba.append(pCF->data+1,7);
        }
        break;
    case 2:
    case 3:
    case 4:
    case 5:
        m_ba.append(pCF->data,8);
        ret = true;
        break;
    case 0x0ff:
        if(pCF->data[7]!=u8_55) break;
        m_ba.append(pCF->data,7);/////////////////// pkt46: 7   pkt57: 2
        m_header = false;// reset header
        len = m_ba.size();
        printBA(m_ba);
        if(m_const57==len){
            ret = true;
            mkFile(m_ba);//

            emit sigCmdInc(1);
        }
        else{
            if(m_flagDebug)qDebug(" frame57(%d) err len=%d",m_const57, len);
        }
        break;
    default:
        ret=false;
        break;
    }

    return ret;
}
