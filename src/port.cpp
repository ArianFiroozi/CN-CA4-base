#include "./headers/port.h"

using namespace std;

Port::Port(QObject *parent)
    : QObject{parent}
{}

void Port::write(Packet _packet)
{
    emit getPacket(_packet.string);
}

void Port::read(QString _packet)
{
    packet.string = _packet;
}

Port::~Port()
{}
