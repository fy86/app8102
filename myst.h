#ifndef MYST_H
#define MYST_H

#include <QObject>

struct myst_can{
    int id32;
    int len;
    char data[8];
};

class myst : public QObject
{
    Q_OBJECT
public:
    explicit myst(QObject *parent = 0);

signals:

public slots:

};

#endif // MYST_H
