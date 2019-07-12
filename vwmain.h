#ifndef VWMAIN_H
#define VWMAIN_H

#include <QObject>

class vwmain : public QObject
{
    Q_OBJECT
public:
    explicit vwmain(QObject *parent = 0);

    int cansend();// can-utils/cansend
    int getCF(int id32,int len,char *p, struct canfd_frame *cf);

signals:

public slots:

};

#endif // VWMAIN_H
