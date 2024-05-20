#ifndef PACKET_H
#define PACKET_H

#include <QString>

enum IPVersion{IPv4, IPv6};

class Packet
{
private:
    QString packet;
public:
    Packet();
    IPVersion IPVer;
    QString getString();
};

#endif // PACKET_H
