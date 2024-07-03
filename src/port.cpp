#include "./headers/port.h"
#include "./headers/common.h"

#include <QMutex>

using namespace std;

Port::Port(int _id, int _delay)
{
    id = _id;
    delay = _delay;
}

void Port::write(QSharedPointer<Packet> _packet)
{
    myMutex.lock();
    _packet->setPortID(id);
    emit getPacket(_packet);
    myMutex.unlock();
}

void Port::read(QSharedPointer<Packet> _packet)
{
    packet = _packet;
}

Port::~Port()
{}

void Port::setDelay(int newDelay)
{
    delay = newDelay;
}
