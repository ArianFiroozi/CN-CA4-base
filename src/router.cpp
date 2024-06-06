#include "./headers/router.h"

#include "../headers/common.h"

void Router::forwardTable()
{
    for (Port* port:ports)
    {
        QString routingTableStr = routingTable.toString(*ip, port->id);
        if (routingTableStr=="")
            continue;
        waitingQueue.append(WaitingQueueLine(port, QSharedPointer<Packet>(
                                                       new Packet(routingTableStr,
                                                                  protocol == RIP?ROUTING_TABLE_RIP:LSA, ipVer, *ip, *ip)),
                                             1, clk));
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

Router::Router(int _id, IPv4 *_ip, RoutingProtocol _protocol, IPVersion _ipVer, int _bufferSize, QThread *parent)
    : QThread{parent},
    ip(_ip),
    routingTable(RoutingTable(ip))
{
    ipVer = _ipVer;
    id = _id;
    protocol = _protocol;
    sendTable = false;
    clk = -1;
    bufferSize = _bufferSize;

    if (ipVer == IPV6)
    {
        ip6 = new IPv6(128);
        *ip6 = ip6->mapFromIPv4(*ip);
    }
}

void Router::getHello(QSharedPointer<Packet> packet)
{
    if (packet->ipVer == IPV6 && this->ipVer==IPV6)
        routingTable.addRoute(Route(packet->getSource(), packet->getSource().mask, IPv4("255.255.255.255", ip->ipAddr.toStr()),
                                    this->getPortWithID(portTranslation(packet->getPortID())), packet->getString().toInt(), IBGP, IPV6));
    else
        routingTable.addRoute(Route(packet->getSource(), packet->getSource().mask, IPv4("255.255.255.255", ip->ipAddr.toStr()),
                                    this->getPortWithID(portTranslation(packet->getPortID())), packet->getString().toInt()));
    sendTable = true;
}

void Router::processMsg(QSharedPointer<Packet> packet)
{
    if (ipVer==IPV6&&packet->getString()=="tunneled")
    {
        qDebug() << "got tunneled message on" <<id;
        buffer.append(packet->tunnelPacket);
    }
    else
        buffer.append(packet);
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
        processMsg(packet);
        break;
    case HELLO:
        getHello(packet);
        break;
    case ROUTING_TABLE_RIP:
        sendTable = routingTable.updateFromPacketRIP(
                        packet->getString(), getPortWithID(portTranslation(packet->getPortID())), clk, ipVer==IPV4?IPV4:packet->ipVer)
                    || sendTable;
        break;
    case LSA:
        sendTable = routingTable.updateFromPacketOSPF(
                        packet->getString(), getPortWithID(portTranslation(packet->getPortID())), ipVer==IPV4?IPV4: packet->ipVer)
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
        sendTable = false;
    }
    else
        forward();

    sendWaiting();
}

QSharedPointer<Packet> Router::tunnelPacket(QSharedPointer<Packet> packet)
{
    qDebug() << "tunneling" << id ;
    QSharedPointer<Packet> tunneledPacket(new Packet("tunneled", packet->getType(), IPV4, packet->getSource(), packet->getDest()));
    tunneledPacket->tunnelPacket = packet;

    return tunneledPacket;
}

bool Router::sendPacket(QSharedPointer<Packet> packet)
{
    IPv4 dest = packet->getDest();

    if (dest.getIPStr() == BROADCAST_ADDRESS)
    {
        // qDebug() << "broadcasting from" << id;

        for (auto port:ports)
            port->write(packet);

        return true;
    }

    Route sendRoute = routingTable.findBestRoute(dest);

    if (!packet->getDest().includes(sendRoute.dest))
    {
        cout<<"router "<<id<<" dropped message!" << packet->getDest().getIPStr().toStdString()<<endl;
        emit packetDropped();
        return false;
    }

    if (sendRoute.ipVer == IPV4 && packet->ipVer == IPV6)
        waitingQueue.append(WaitingQueueLine(sendRoute.port, tunnelPacket(packet), sendRoute.port->delay, clk));
    else
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
