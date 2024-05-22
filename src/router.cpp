#include "./headers/router.h"

Router::Router(int _id, QThread *parent)
    : QThread{parent}
{
    id = _id;
}

void Router::recievePacket(Packet *packet, int recieveID)
{
    // infinite buffer
    buffer.append(packet);
}

bool Router::sendPacket(Packet *packet, int sendID)
{
    IPv4 dest = packet->getDest();
    Route sendRoute = routingTable.findBestRoute(&dest);
    sendRoute.port->write(*packet);
    return true;
}
