#include "parserimg.h"

parserimg::parserimg(QObject *parent) :
    parserfile(parent)
{
    m_nFirstFrame = 0;

    m_const57 = 256;

    m_nFrame1st0 = 0xf07f4324;// 0xf07fe320;// 0xf07f2321;
    m_nFrame1st1 = 0x2af0ff11;// 0x8ef0ff11;// 0x4df0ff11;
    m_nID3200 =  0x1911f000;

    strcpy(m_szFileName,"img00");
    loadFileImg();

}

// id6 : 1A.cf.fc.7d.00.ff

bool parserimg::mk57(myst_can *pCF)
{
    char u8_3c=(char)0x3c;
    bool ret = false;
    int len;

    //qDebug(" parse img , mk57");

    int id24 = m_nID3200;
    int m24 = 0xffffff00;
    int m=pCF->id32 & m24;
    int sn;
    if(m!=id24){
        return ret;
    }
    sn = 0x0ff & pCF->id32;

    if(sn==1){
        if(pCF->data[0]!=u8_3c){
            ret = false;
        }
        else{
            ret = true;
            m_ba.clear();// init redo
            m_ba.append(pCF->data+1,7);////////////////  diff frame57
        }
    }
    else if(0x0ff == sn){
        m_ba.append(pCF->data,1);/////////////////// pkt46: 7   pkt57: 2
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
    }
    else if( sn>1 && sn<=0x20){
        m_ba.append(pCF->data,8);
        ret = true;
    }
    else {
        ret = false;
    }

    return ret;

}
// id6 : 1A.cf.fc.7d.00.ff

bool parserimg::isImgHeader(QByteArray ba)
{
    bool ret=false;

    char szHeader[10];
    szHeader[0]=(char)0x1a;
    szHeader[1]=(char)0xcf;
    szHeader[2]=(char)0xfc;
    szHeader[3]=(char)0x7d;
    szHeader[4]=(char)0x00;
    szHeader[5]=(char)0xff;

    if(0==memcmp(szHeader,ba.data(),6)){
        qDebug(" img header found");
        ret=true;
    }

    return ret;
}

bool parserimg::mkFile(QByteArray ba)
{
    bool ret=true;
    int sn;
    int i0;

    //i0=ba.at(0);////////////// fixme
    //sn=i0 & 0x0ff;
    //if(sn==0){

    emit sigCmdInc(1);

    if(isImgHeader(ba)){
        ret = initFile(ba);
    }
    else{
        appendBAfile(ba);
    }
    m_nFirstFrame++;


    return ret;
}
// set file name, len
bool parserimg::initFile(QByteArray ba)
{
    bool ret=true;
    int i0,i1,i2,i3;
    int ncp,nleft;

    m_nPktFile = 9999;

    m_nLenFile = 256*9999;

    //strcpy(m_szFileName,"img00");

    if(m_flagDebug) qDebug("file: %s -- pkt: %d , len: %d",m_szFileName,m_nPktFile,m_nLenFile);


    m_baFile.clear();
    m_baFile.append(ba.data(),256);

    saveFile();

    return ret;
}

int parserimg::appendBAfile(QByteArray ba)
{
#if 0
    int sn;
    int i0;
    int ncp,nleft;

    i0=ba.at(0);
    sn=i0 & 0x0ff;
    if(sn<1) return -1;
#endif
    //if(sn<m_nPktFile){
        m_baFile.append(ba.data(),256);
        saveFile();
    //}
    //else{
        //if(m_flagDebug)qDebug(" error file pkt.sn too big XXXXXXXXXXXXXXXXXXx");
        //return -2;
    //}

    return 0;


}
void parserimg::loadFileImg()
{

    QByteArray baFullName;

    baFullName.append(m_szDir);
    baFullName.append(m_szFileName);

    QFile file(baFullName);

    if(file.exists()){
        if(file.open(QIODevice::ReadOnly)){
            m_baFile=file.readAll();
            file.close();
        }
    }
    else{
        m_baFile.clear();
    }

}

int parserimg::saveFile()
{
    int ret=0;


    QByteArray baFullName;

    baFullName.append(m_szDir);
    baFullName.append(m_szFileName);

    QFile file(baFullName);
    file.remove();
    if(file.open(QIODevice::ReadWrite)){
        file.write(m_baFile);
        file.close();

        if(m_flagDebug)qDebug(" file save: %s",m_szFileName);
    }


    return ret;

}

