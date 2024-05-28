#include "./headers/router.h"

int portTranslation(int other)
{
    switch (other)
    {
    case 1:
        return 3;
    case 2:
        return 4;
    case 3: // handler to star seperatley
        return 1;
    case 4:
        return 2;
    default:
        return other; //star
    }
}


void Router::forwardTable()
{
    for (Port* port:ports)
    {
        port->write(QSharedPointer<Packet>(new Packet(routingTable.toStringRIP(*ip), ROUTING_TABLE_RIP, IPV4, *ip, *ip)));
    }
}

Router::Router(int _id, IPv4 *_ip, RoutingProtocol _protocol, QThread *parent)
    : QThread{parent},
    ip(_ip),
    routingTable(RoutingTable(ip))
{
    id = _id;
    protocol = _protocol;
    sendTable = false;
}

void Router::recievePacket(QSharedPointer<Packet> packet)
{
    // infinite buffer

    switch(packet->getType()){
    case MSG:
        routingTable.print();
        cout<<"router "<<id<<" recieved msg!" << packet->getDest().getIPStr().toStdString()<<endl;
        buffer.append(packet);
        break;
    case HELLO:
        cout<<"router "<<id<<" recieved hello!"<<endl;
        if (protocol == RIP)
        {
            routingTable.addRoute(Route(packet->getSource(), packet->getSource().mask, *ip,
                                        this->getPortWithID(portTranslation(packet->getPortID())), packet->getString().toInt()));
            sendTable = true;

            // cout<< "hello message:" <<packet->getString().toStdString()<<endl<<endl;
        }
        break;
    case ROUTING_TABLE_RIP:
        cout<<"router "<<id<<" recieved routing table!"<<endl;
        // cout << "before:\t" << routingTable.toStringRIP(*ip).toStdString()<<endl;
        sendTable = routingTable.updateFromPacketRIP(packet->getString(), getPortWithID(portTranslation(packet->getPortID()))) || sendTable;
        // cout << "after:\t" << routingTable.toStringRIP(*ip).toStdString()<<endl;
        // cout<< "routing table msg:" <<packet->getString().toStdString()<<endl <<endl;
        break;
    default:
        cout<<"router "<<id<<" recieved something!"<<endl;
        cerr << "unknown message type!" << endl;
    }
}

void Router::forward()
{
    if (buffer.size()==0) return;

    buffer[0]->addToPath(QString::number(this->id));
    sendPacket(buffer[0]);
    buffer.removeFirst();

    for (QSharedPointer<Packet> packet : buffer)
        packet->incQueueWaitCycles();
}

void Router::tick()
{
    for (int i=0; i<buffer.size(); i++)
        buffer[i]->incWaitCycles();

    if (sendTable)
    {
        forwardTable();
        sendTable = false;
    }
    else
        forward();
}

bool Router::sendPacket(QSharedPointer<Packet> packet)
{
    IPv4 dest = packet->getDest();
    Route sendRoute = routingTable.findBestRoute(dest);

    if (!packet->getDest().includes(sendRoute.dest))
    {
        cout<<"router "<<id<<" dropped message!"<<endl;
        return false;
    }

    cout<<"router "<<id<<" sended message through: "<<sendRoute.port->id
         << " to->" << sendRoute.gateway.getIPStr().toStdString()<<endl;
    // cout << "message content was:" << packet->getString().toStdString() <<endl;

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


