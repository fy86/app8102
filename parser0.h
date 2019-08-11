#ifndef PARSER0_H
#define PARSER0_H

#include <QObject>

#include <stdio.h>
#include "myst.h"

class parser0 : public QObject
{
    Q_OBJECT
public:
    explicit parser0(QObject *parent = 0);

    bool m_flagDebug;// for qDebug

    bool m_header;
    int m_nFrame1st0;
    int m_nFrame1st1;
    int m_nID3200;
    QByteArray m_ba;

    bool checkHeader(struct myst_can *pCF);

    virtual bool parse(struct myst_can *pCF);


    void printBA(QByteArray ba);// for test
signals:
    void sigUdpLog(QByteArray ba);

public slots:

};

#endif // PARSER0_H
