#ifndef CANRECVTHREAD_H
#define CANRECVTHREAD_H

#include <QObject>
#include <QThread>
#include <QByteArray>
#include <QUdpSocket>

#include "myst.h"

class canrecvthread : public QThread
{
    Q_OBJECT
public:
    explicit canrecvthread(QObject *parent = 0);

protected:
    void run();

signals:
    void sigUdpLog(QByteArray ba);

public slots:

};

#endif // CANRECVTHREAD_H
