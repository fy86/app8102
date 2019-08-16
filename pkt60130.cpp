#include "pkt60130.h"

pkt60130::pkt60130(QObject *parent) :
    parserfile(parent)
{
    m_const57 = 98;

    m_nFrame1st0 = 0xf02fb321;// 0xf02f1f21;// 0xf07fe320;// 0xf07f2321;
    m_nFrame1st1 = 0x1ef1ffff;// 0x62f1ffff;// 0x8ef0ff11;// 0x4df0ff11;
    m_nID3200 = 0x19fff000;  // broadcast addr

}
bool pkt60130::mk57(myst_can *pCF)
{
    char u8_1b=(char)0x1b;
    char u8_01=(char)0x01;
    bool ret = false;
    int len;

    int id24 = m_nID3200;
    int m24 = 0xffffff00;
    int m=pCF->id32 & m24;
    int sn;
    if(m!=id24){
        return ret;
    }
    sn = 0x0ff & pCF->id32;

    switch(sn){
    case 1:
        if(pCF->data[0]!=u8_1b){
            ret = false;
        }
        else if(pCF->data[1] != u8_01){
            ret = false;
        }
        else{
            ret = true;
            m_ba.clear();// init redo
            m_ba.append(pCF->data+2,6);////////////////  diff frame57
        }
        break;
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 0x0a:
    case 0x0b:
    case 0x0c:
        m_ba.append(pCF->data,8);
        ret = true;
        break;
    case 0x0ff:
        m_ba.append(pCF->data,4);/////////////////// pkt46: 7   pkt57: 2
        m_header = false;// reset header
        len = m_ba.size();
        printBA(m_ba);
        if(m_const57==len){
            ret = true;
            mkFile(m_ba);//
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

bool pkt60130::mkFile(QByteArray ba)
{
    bool ret=false;
    int sn;
    int i0;

    emit sigCmdInc(1);

    i0=ba.at(0);
    sn=i0 & 0x0ff;
    if(sn==0){
        ret = initFile(ba);
        return ret;
    }

    appendBAfile(ba);

    return ret;
}
bool pkt60130::initFile(QByteArray ba)
{
    bool ret=true;
    int i0,i1,i2,i3;
    int ncp,nleft;

    qDebug(" pkt600130 init file");

    m_nPktFile = 130;

    m_nLenFile = 130 * m_const57;// 60==>98

    strcpy(m_szFileName,"tel100.bin");

    if(m_flagDebug) qDebug("file: %s -- pkt: %d , len: %d",m_szFileName,m_nPktFile,m_nLenFile);


    m_baFile.clear();
    m_baFile.append(ba.data(),m_const57);

    return ret;
}
int pkt60130::appendBAfile(QByteArray ba)
{
    int sn;
    int i0;
    int ncp,nleft;
    char pCmd[80];
    int r=0;

    qDebug(" pkt600130 append file");

    sprintf(pCmd,"/usr/Python34/bin/python3 /home/root/qt/Python01.py");


    i0=ba.at(0);
    sn=i0 & 0x0ff;
    if(sn<1) return -1;

    if(sn<m_nPktFile){
        m_baFile.append(ba.data(),m_const57);
        r = saveFile();

        if(r==1) m_process.execute(pCmd);
    }
    else{
        if(m_flagDebug)qDebug(" error file pkt.sn too big XXXXXXXXXXXXXXXXXXx");
        return -2;
    }

    return 0;
}


