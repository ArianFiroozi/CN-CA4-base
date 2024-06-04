#include "./headers/port.h"

using namespace std;

Port::Port(int _id, int _delay)
{
    id = _id;
    delay = _delay;
}

void Port::write(QSharedPointer<Packet> _packet)
{
    mutex.lock();
    _packet->setPortID(id);
    emit getPacket(_packet);
    mutex.unlock();
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
