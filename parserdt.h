#ifndef PARSERDT_H
#define PARSERDT_H

#include <QObject>
#include <QDateTime>
#include <QProcess>
#include <QLocale>

#include "parser0.h"

class parserDT : public parser0
{
    Q_OBJECT
public:
    explicit parserDT(QObject *parent = 0);

    int m_nSet;
    QProcess m_process;

    void setDT(struct myst_can *pCF);
    void cmdDT(unsigned int dt32);

    virtual bool parse(struct myst_can *pCF);

signals:

public slots:

};

#endif // PARSERDT_H
