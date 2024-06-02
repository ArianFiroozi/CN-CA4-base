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
        cout<<"router "<<id<<" dropped message!"<<endl;
        emit packetDropped();
        return;
    }

    switch(packet->getType()){
    case MSG:
        // routingTable.print();
        // qDebug()<<"router "<<id<<" recieved msg!" ;
        buffer.append(packet);
        break;
    case HELLO:
        // qDebug()<<"router "<<id<<" recieved hello!";
        if (protocol == RIP) // no need for protocol
        {
            routingTable.addRoute(Route(packet->getSource(), packet->getSource().mask, *ip,
                                        this->getPortWithID(portTranslation(packet->getPortID())), packet->getString().toInt()));
            sendTable = true;
            doNotSend.append(portTranslation(packet->getPortID()));
        }
        else if (protocol == OSPF)
        {
            routingTable.addRoute(Route(packet->getSource(), packet->getSource().mask, *ip,
                                        this->getPortWithID(portTranslation(packet->getPortID())), packet->getString().toInt()));
            sendTable = true;
            doNotSend.append(portTranslation(packet->getPortID()));
        }
        break;
    case ROUTING_TABLE_RIP:
        // qDebug() <<"router "<<id<<" recieved routing table!";
        sendTable = routingTable.updateFromPacketRIP(packet->getString(), getPortWithID(portTranslation(packet->getPortID())), clk) || sendTable;
        // cout<< "routing table msg:" <<packet->getString().toStdString()<<endl <<endl;
        break;
    case LSA:
        // qDebug() <<"router "<<id<<" recieved LSA!";
        sendTable = routingTable.updateFromPacketOSPF(packet->getString(), getPortWithID(portTranslation(packet->getPortID()))) || sendTable;
        break;
    default:
        cerr << "unknown message type on router:" << id << endl;
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
    Route sendRoute = routingTable.findBestRoute(dest);

    if (!packet->getDest().includes(sendRoute.dest))
    {
        cout<<"router "<<id<<" dropped message!"<<endl;
        emit packetDropped();
        return false;
    }

    // if (!this->ip->includes(sendRoute.gateway))
    //     qDebug() << "external send!"<<this->ip->getIPStr() << sendRoute.gateway.getIPStr();

    // cout<<"router "<<id<<" sended message through: "<<sendRoute.port->id
    // << " to->" << sendRoute.gateway.getIPStr().toStdString()<<endl;
    // cout << "message content was:" << packet->getString().toStdString() <<endl;

    waitingQueue.append(WaitingQueueLine(sendRoute.port, packet, sendRoute.port->delay, clk));
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

WaitingQueueLine::WaitingQueueLine(Port *port, QSharedPointer<Packet> packet, int delay, int queueTime) :
    port(port),
    packet(std::move(packet)),
    delay(delay),
    queueTime(queueTime)
{}
