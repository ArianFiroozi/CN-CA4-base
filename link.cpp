#include "link.h"

Link::Link(QObject *parent)
    : QThread{parent}
{}

void Link::write(Packet _packet)
{
    QMutexLocker locker(&mutex);
    packet = _packet;
    ready.wakeOne();
}

Packet Link::read()
{
    QMutexLocker locker(&mutex);
    while (!ready.wait(&mutex));
    return packet;
}
