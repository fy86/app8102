#ifndef PARSER7_H
#define PARSER7_H

#include <QObject>

class parser7 : public QObject
{
    Q_OBJECT
public:
    explicit parser7(QObject *parent = 0);

    bool m_bHeader;// first frame received

signals:

public slots:

};

#endif // PARSER7_H
