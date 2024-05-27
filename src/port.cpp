#include "./headers/port.h"

using namespace std;

Port::Port(int _id)
{
    id = _id;
}

void Port::write(QSharedPointer<Packet> _packet)
{
    emit getPacket(_packet);
}

void Port::read(QSharedPointer<Packet> _packet)
{
    packet = _packet;
}

Port::~Port()
{}
