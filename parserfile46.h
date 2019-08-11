#ifndef PARSERFILE46_H
#define PARSERFILE46_H

#include <QObject>

#include <parserfile.h>

class parserfile46 : public parserfile
{
    Q_OBJECT
public:
    explicit parserfile46(QObject *parent = 0);

    virtual bool mk57(struct myst_can *pCF);

signals:

public slots:

};

#endif // PARSERFILE46_H
