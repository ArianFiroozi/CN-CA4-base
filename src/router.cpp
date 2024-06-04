#include "./headers/router.h"

#include "../headers/common.h"

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
    if (protocol == RIP)
    {
        for (Port* port:ports)
            if (!doNotSend.contains(port->id))
                waitingQueue.append(WaitingQueueLine(port,QSharedPointer<Packet>(new Packet(routingTable.toStringRIP(*ip), ROUTING_TABLE_RIP, IPV4, *ip, *ip)),
                                                     port->delay, clk));
    }
    else if (protocol == OSPF)
    {
        for (Port* port:ports)
            if (!doNotSend.contains(port->id))
                waitingQueue.append(WaitingQueueLine(port,QSharedPointer<Packet>(new Packet(routingTable.toStringOSPF(*ip), LSA, IPV4, *ip, *ip)),
                                                     port->delay, clk));
    }
}

void Router::sendWaiting()
{
    QVector<int> sent;
    for (int i = 0; i < waitingQueue.size();i++)
        if (clk >= waitingQueue[i].delay + waitingQueue[i].queueTime)
        {
            waitingQueue[i].port->write(waitingQueue[i].packet);
            sent.append(i);
        }

    for (int i = sent.size()-1; i>=0 ; i--)
        waitingQueue.remove(i);
}

Router::Router(int _id, IPv4 *_ip, RoutingProtocol _protocol, int _bufferSize, QThread *parent)
    : QThread{parent},
    ip(_ip),
    routingTable(RoutingTable(ip))
{
    id = _id;
    protocol = _protocol;
    sendTable = false;
    clk = -1;
    bufferSize = _bufferSize;
}

void Router::recievePacket(QSharedPointer<Packet> packet)
{
    if (buffer.size() >= bufferSize && bufferSize != -1)
    {
        cout<<"router "<<id<<" dropped message, congested!"<<endl;
        emit packetDropped();
        return;
    }

    // loop detection for broadcast
    if (packet->getDest().getIPStr() == BROADCAST_ADDRESS && packet->getPath().contains(QString::number(id)))
        return;

    switch(packet->getType()){
    case MSG:
        buffer.append(packet);
        break;
    case HELLO:
        routingTable.addRoute(Route(packet->getSource(), packet->getSource().mask, IPv4("255.255.255.255", ip->ipAddr.toStr()),
                                    this->getPortWithID(portTranslation(packet->getPortID())), packet->getString().toInt()));
        sendTable = true;
        doNotSend.append(portTranslation(packet->getPortID()));
        break;
    case ROUTING_TABLE_RIP:
        sendTable = routingTable.updateFromPacketRIP(
                        packet->getString(), getPortWithID(portTranslation(packet->getPortID())), clk)
                    || sendTable;
        break;
    case LSA:
        sendTable = routingTable.updateFromPacketOSPF(
                        packet->getString(), getPortWithID(portTranslation(packet->getPortID())))
                    || sendTable;
        break;
    case DHCP_LEASE:
    case DHCP_REQUEST:
    case DHCP_OFFER:
        // qDebug() << "router" << id << "got dhcp";
        buffer.append(packet);
        break;

    default:
        cerr << "unknown message type on router:" << id << endl;
    }
}

void Router::forward()
{
    if (buffer.size()==0)
        return;

    while (buffer[0]->getDest().getIPStr() == BROADCAST_ADDRESS &&
           buffer[0]->getPath().contains(QString::number(this->id))) // broadcast loop
    {
        buffer.removeFirst();
        if (buffer.size()==0)
            return;
    }

    buffer[0]->addToPath(QString::number(this->id));
    sendPacket(buffer[0]);
    buffer.removeFirst();

    for (QSharedPointer<Packet> packet : buffer)
        packet->incQueueWaitCycles();
}

void Router::tick(int _time)
{
    clk = _time;
    for (int i=0; i<buffer.size(); i++)
        buffer[i]->incWaitCycles();

    for (int i=0; i<waitingQueue.size(); i++)
        waitingQueue[i].packet->incWaitCycles();

    if (protocol==RIP)
    {
        routingTable.removeTimeOutRoutes(clk);
        sendTable = sendTable || !(clk%RIP_SEND_ROUTING_TABLE);
    }

    if (sendTable)
    {
        forwardTable();
        doNotSend = QVector<int> ();
        sendTable = false;
    }
    else
        forward();

    sendWaiting();
}

bool Router::sendPacket(QSharedPointer<Packet> packet)
{
    IPv4 dest = packet->getDest();

    if (dest.getIPStr() == BROADCAST_ADDRESS)
    {
        // qDebug() << "broadcasting from" << id;

        for (auto port:ports)
            port->write(packet);
        // waitingQueue.append(WaitingQueueLine(port, packet, port->delay, clk));

        return true;
    }

    Route sendRoute = routingTable.findBestRoute(dest);

    if (!packet->getDest().includes(sendRoute.dest))
    {
        cout<<"router "<<id<<" dropped message!" << packet->getDest().getIPStr().toStdString()<<endl;
        emit packetDropped();
        return false;
    }

    waitingQueue.append(WaitingQueueLine(sendRoute.port, packet, sendRoute.port->delay, clk));
    return true;
}

void Router::addPort(Port* newPort)
{
    for (auto port:ports)
        if (port->id == newPort->id)
            exit(0);
    ports.append(newPort);
}

Port* Router::getPortWithID(int portID)
{
    for (Port* port : ports)
        if (port->id == portID)
            return port;
    return NULL;
}

WaitingQueueLine::WaitingQueueLine(Port *port, QSharedPointer<Packet> packet, int delay, int queueTime) :
    port(port),
    packet(std::move(packet)),
    delay(delay),
    queueTime(queueTime)
{}
