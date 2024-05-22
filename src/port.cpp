#include "./headers/port.h"

using namespace std;

Port::Port(int _id)
{
    id = _id;
}

void Port::write(Packet _packet)
{
    emit getPacket(_packet);
}

void Port::read(Packet _packet)
{
    packet = _packet;
}

Port::~Port()
{}
