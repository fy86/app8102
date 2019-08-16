#ifndef PARSERFILE_H
#define PARSERFILE_H

#include <QObject>
#include <QFile>
#include <QProcess>
#include <QDir>

#include "parser0.h"
#include "myst.h"

class parserfile : public parser0
{
    Q_OBJECT
public:
    explicit parserfile(QObject *parent = 0);

    QByteArray m_baFile;

    virtual bool parse(struct myst_can *pCF);

    char m_szDir[32];

    int m_const57;// normal 57


    int m_nPktFile;
    int m_nLenFile;
    char m_szFileName[32];
    virtual bool mkFile(QByteArray ba);
    virtual bool initFile(QByteArray ba);
    virtual int saveFile();
    bool isSH(char *pfilename);
    int runFile();
    virtual int appendBAfile(QByteArray ba);

    virtual bool mk57(struct myst_can *pCF);


signals:

public slots:
    void slotRunFile(QString filenameFull);

};

#endif // PARSERFILE_H
