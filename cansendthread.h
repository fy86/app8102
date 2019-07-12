#ifndef CANSENDTHREAD_H
#define CANSENDTHREAD_H

#include <QObject>
#include <QThread>
#include <QSemaphore>
#include <QQueue>

#include "myst.h"

class cansendthread : public QThread
{
    Q_OBJECT
public:
    explicit cansendthread(QObject *parent = 0);

    bool m_bOpen;
    int m_sock;

    QQueue<QByteArray> m_qBA0;// first check
    QQueue<QByteArray> m_qBA1;
    QSemaphore m_sem;

    int openCAN(char *pCAName);
    int getCF(struct canfd_frame *cf);
    int canSend();

protected:
    void run();

signals:

public slots:
    void slotQadd(QByteArray ba,int qn);

};

#endif // CANSENDTHREAD_H
