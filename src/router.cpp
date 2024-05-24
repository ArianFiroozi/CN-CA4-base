#include "./headers/router.h"

Router::Router(int _id, QThread *parent)
    : QThread{parent}
{
    id = _id;
}

void Router::recievePacket(Packet packet)
{
    // infinite buffer
    buffer.append(packet);
    cout<<"router "<<id<<" recieved smt!"<<endl;
}

void Router::forward()
{
    if (buffer.size()==0) return;

    sendPacket(buffer[0]);
    buffer.removeFirst();
}

bool Router::sendPacket(Packet packet)
{
    IPv4 dest = packet.getDest();
    Route sendRoute = routingTable.findBestRoute(dest);

    if (!packet.getDest().includes(sendRoute.dest))
    {
        cout<<"router "<<id<<" dropped message!"<<endl;
        return false;
    }

    cout<<"router "<<id<<" sended message through: "<<sendRoute.port->id
         << " to->" << sendRoute.gateway.getIPStr().toStdString()<<endl;
    sendRoute.port->write(packet);
    return true;
}

void Router::addPort(Port* newPort)
{
    ports.append(newPort);
}

Port* Router::getPortWithID(int portID)
{
    for (Port* port : ports)
        if (port->id == portID)
            return port;
    return NULL;
}


