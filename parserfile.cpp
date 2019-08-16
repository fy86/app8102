#include "parserfile.h"

parserfile::parserfile(QObject *parent) :
    parser0(parent)
{
    m_const57 = 57;

    m_header = false;
    m_nFrame1st0 = 0xf07f2321;
    m_nFrame1st1 = 0x4df0ff11;
    m_nID3200 = 0x1911f000;

    m_nPktFile = 0;
    m_nLenFile = 0;

    strcpy(m_szDir,"/home/root/qt/");
}

bool parserfile::parse(myst_can *pCF)
{
    int ret=false;

    ret = checkHeader(pCF);
    //
    if(ret){ // init frame57
        m_ba.clear();
    }
    else if(m_header){  // frame.data
        ret = mk57(pCF);
    }

    return ret;
}

bool parserfile::mk57(myst_can *pCF)
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
    case 6:
    case 7:
        m_ba.append(pCF->data,8);
        ret = true;
        break;
    case 0x0ff:
        if(pCF->data[2]!=u8_55) break;
        if(pCF->data[3]!=u8_55) break;
        if(pCF->data[4]!=u8_55) break;
        if(pCF->data[5]!=u8_55) break;
        if(pCF->data[6]!=u8_55) break;
        if(pCF->data[7]!=u8_55) break;
        m_ba.append(pCF->data,2);
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
int parserfile::appendBAfile(QByteArray ba)
{
    int sn;
    int i0,i1;
    int ncp,nleft;

    i0=ba.at(0);
    i1=ba.at(1);
    sn=i1 & 0x0ff;
    sn |= 0x0ff00 & (i0<<8);
    if(sn<1) return -1;

    if(sn<m_nPktFile){
        ncp = ba.size()-2;
        nleft = m_nLenFile - m_baFile.size();
        if(ncp > nleft) ncp = nleft;
        m_baFile.append(ba.data()+2,ncp);
        saveFile();
    }
    else{
        if(m_flagDebug)qDebug(" error file pkt.sn too big XXXXXXXXXXXXXXXXXXx");
        return -2;
    }

    return 0;
}

bool parserfile::mkFile(QByteArray ba)
{
    bool ret=false;
    int sn;
    int i0,i1;

    i0=ba.at(0);
    i1=ba.at(1);
    sn=i1 & 0x0ff;
    sn |= 0x0ff00 & (i0<<8);
    if(sn==0){
        ret = initFile(ba);
        return ret;
    }

    appendBAfile(ba);

    return ret;
}

bool parserfile::initFile(QByteArray ba)
{
    bool ret=true;
    int i0,i1,i2,i3;
    int ncp,nleft;

    i0=ba.at(2);
    i1=ba.at(3);
    m_nPktFile = i1 & 0x0ff;
    m_nPktFile |= 0x0ff00 & (i0<<8);

    i0 = ba.at(4);
    i1 = ba.at(5);
    i2 = ba.at(6);
    i3 = ba.at(7);
    m_nLenFile = 0x0ff & i3;
    m_nLenFile |= 0x0ff00 & (i2<<8);
    m_nLenFile |= 0x0ff0000 & (i1<<16);
    m_nLenFile |=  0xff000000 & (i0<<24);

    memcpy(m_szFileName,ba.data()+8,12);
    m_szFileName[12]=0;

    if(m_flagDebug) qDebug("file: %s -- pkt: %d , len: %d",m_szFileName,m_nPktFile,m_nLenFile);

    m_baFile.clear();
    nleft=m_const57-2-2-4-12;
    ncp=m_nLenFile-nleft;

    if(ncp>=0)ncp=nleft;
    else ncp = m_nLenFile;

    m_baFile.append(ba.data()+2+2+4+12,ncp);

    if(m_nLenFile==m_baFile.size()) saveFile();

    return ret;
}
int parserfile::runFile()
{

    return 0;
}

void parserfile::slotRunFile(QString filenameFull)
{
    //QDir d(filenameFull);
    //QString fn=getFullName(d.dirName());
    QString strBA;
    //QStringList sl;
    if(!QFile(filenameFull).exists())return;

    //if(isBash(fn)){
        QProcess *m_pProcess = new QProcess();
        strBA=QString("/bin/sh ")+ filenameFull +QString(" &");
        //syslog(LOG_INFO," run bash :   %s",strBA.toLatin1().data());
        m_pProcess->start(strBA);
    //}

}

int parserfile::saveFile()
{
    int ret=0;


    QByteArray baFullName;

    if(m_nLenFile!=m_baFile.size()){
        qDebug(" len !=   len:%d    size:%d   ",m_nLenFile,m_baFile.size());
        return 0;
    }

    baFullName.append(m_szDir);
    baFullName.append(m_szFileName);

    if(isSH(m_szFileName)){
        m_baFile.replace('\r','\n');
    }

    QFile file(baFullName);
    file.remove();
    if(file.open(QIODevice::ReadWrite)){
        file.write(m_baFile);
        file.close();

        if(m_flagDebug)qDebug(" file save: %s",m_szFileName);
        if(isSH(m_szFileName)){
            slotRunFile(QString(baFullName));
        }

        ret = 1;
    }


    return ret;
}
bool parserfile::isSH(char *pfilename)
{
    int ret=false;
    int len=strlen(pfilename);
    if(len<4) return ret;
    if(0==strcmp(".sh",pfilename+len-3)){
        return true;
    }
    return ret;
}


