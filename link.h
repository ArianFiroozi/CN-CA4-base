#ifndef LINK_H
#define LINK_H

#include <QThread>
#include <QCoreApplication>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>

#include "packet.h"

class Link : public QThread
{
private:
    QMutex mutex;
    QWaitCondition ready;
    Packet packet;

public:
    explicit Link(QObject *parent = nullptr);

    void write(Packet _packet);
    Packet read();
};

#endif // LINK_H
