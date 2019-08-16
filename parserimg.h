#ifndef PARSERIMG_H
#define PARSERIMG_H

#include <QObject>

#include "parserfile.h"

class parserimg : public parserfile
{
    Q_OBJECT
public:
    explicit parserimg(QObject *parent = 0);

    bool m_nFirstFrame;

    virtual bool mk57(struct myst_can *pCF);
    virtual bool mkFile(QByteArray ba);
    virtual bool initFile(QByteArray ba);
    virtual int appendBAfile(QByteArray ba);

    virtual int saveFile();

    void loadFileImg();
    bool isImgHeader(QByteArray ba);

signals:

public slots:

};

#endif // PARSERIMG_H
