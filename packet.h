#ifndef PACKET_H
#define PACKET_H

#include <string>

enum IPVersion{IPv4, IPv6};

class Packet
{
public:
    Packet();
    IPVersion IPVer;
    std::string getString();
};

#endif // PACKET_H
