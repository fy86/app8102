#ifndef PKT60130_H
#define PKT60130_H

#include <QObject>
#include <QByteArray>

#include "parserfile.h"

class pkt60130 : public parserfile
{
    Q_OBJECT
public:
    explicit pkt60130(QObject *parent = 0);

    virtual bool mk57(struct myst_can *pCF);
    virtual bool mkFile(QByteArray ba);
    virtual bool initFile(QByteArray ba);
    virtual int appendBAfile(QByteArray ba);

signals:

public slots:

};

#endif // PKT60130_H
