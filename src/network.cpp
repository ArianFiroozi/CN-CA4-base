#include "../headers/network.h"

Network::Network(EventHandler* _eventHandler, RoutingProtocol protocol, int lambda)
{
    eventHandler = _eventHandler;
    running = false;

    ringStar = new RingStar(7, {2, 4, 6, 7}, IPv4("255.255.255.0", "10.0.0.0"), protocol, true);
    mesh = new Mesh(4, 4, IPv4("255.255.255.0", "20.0.0.0"), protocol, true);

    eventThread = new QThread();
    eventHandler->moveToThread(eventThread);

    createSenders();
    createReceivers();
    addMeshPorts();
    connectRingStar();
    connectMesh();

    ringStar->connectTick(eventHandler);
    mesh->connectTick(eventHandler);

    messagingSystem = new MessagingSystem(lambda, receivers, senders);
    packetsSent = packetsReceived = totalWaitCycles = totalQueueWaitCycles = packetsDropped = 0;
    highestQueueWait = highestWait = -1;
    leastWait = leastQueueWait = 100000;
}

Network::~Network()
{
    delete mesh;
    delete ringStar;
    for (auto receiver:receivers)
        delete receiver;
    for (auto sender:senders)
        delete sender;
    delete eventHandler;
    delete eventThread;
}

int Network::getPacketsSent() const
{
    return packetsSent;
}

int Network::getPacketsReceived() const
{
    return packetsReceived;
}

int Network::getTotalWaitCycles() const
{
    return totalWaitCycles;
}

int Network::getTotalQueueWaitCycles() const
{
    return totalQueueWaitCycles;
}

int Network::getPacketsDropped() const
{
    return packetsDropped;
}

void Network::printRoutingTables()
{
    mesh->printRoutingTables();
    ringStar->printRoutingTables();
}

int Network::getHighestQueueWait() const
{
    return highestQueueWait;
}

int Network::getHighestWait() const
{
    return highestWait;
}

int Network::getLeastWait() const
{
    return leastWait;
}

int Network::getLeastQueueWait() const
{
    return leastQueueWait;
}

void Network::packetReceived(QSharedPointer<Packet> packet)
{
    packetsReceived++;
    totalQueueWaitCycles += packet->getQueueWaitCycles();
    totalWaitCycles += packet->getWaitCycles();

    if (packet->getQueueWaitCycles()>highestQueueWait)
        highestQueueWait = packet->getQueueWaitCycles();
    if (packet->getWaitCycles()>highestWait)
        highestWait = packet->getWaitCycles();
    if (packet->getWaitCycles()<leastWait)
        leastWait = packet->getWaitCycles();
    if (packet->getQueueWaitCycles()<leastQueueWait)
        leastQueueWait = packet->getQueueWaitCycles();

}

void Network::packetSent()
{
    packetsSent++;
}

void Network::packetDropped()
{
    packetsDropped++;
}

void Network::createSenders()
{
    senders.append(new PC(1, new IPv4("255.255.255.255", "192.168.10.1"), new Port(21, 10)));
    senders.append(new PC(2, new IPv4("255.255.255.255", "192.168.10.2"), new Port(22, 10)));
    senders.append(new PC(3, new IPv4("255.255.255.255", "192.168.10.3"), new Port(23, 10)));
    senders.append(new PC(4, new IPv4("255.255.255.255", "192.168.10.4"), new Port(21, 10)));
    senders.append(new PC(5, new IPv4("255.255.255.255", "192.168.10.5"), new Port(22, 10)));


    for (auto sender: senders)
        QObject::connect(sender, &PC::packetSent,
                         this, &Network::packetSent);
}

void Network::createReceivers()
{
    receivers.append(new PC(1, new IPv4("255.255.255.255", "192.168.20.1"), new Port(1, 10)));
    receivers.append(new PC(2, new IPv4("255.255.255.255", "192.168.20.2"), new Port(5, 10)));
    receivers.append(new PC(3, new IPv4("255.255.255.255", "192.168.20.3"), new Port(1, 10)));
    receivers.append(new PC(4, new IPv4("255.255.255.255", "192.168.20.4"), new Port(5, 10)));
    receivers.append(new PC(5, new IPv4("255.255.255.255", "192.168.20.5"), new Port(1, 10)));
    receivers.append(new PC(6, new IPv4("255.255.255.255", "192.168.20.6"), new Port(5, 10)));
    receivers.append(new PC(7, new IPv4("255.255.255.255", "192.168.20.7"), new Port(1, 10)));
    receivers.append(new PC(8, new IPv4("255.255.255.255", "192.168.20.8"), new Port(5, 10)));

    for (auto receiver: receivers)
        QObject::connect(receiver, &PC::packetReceived,
                         this, &Network::packetReceived);
}

void Network::addMeshPorts()
{
    mesh->routers[0]->addPort(new Port(1, 10));
    mesh->routers[1]->addPort(new Port(1, 10));
    mesh->routers[2]->addPort(new Port(1, 10));

    mesh->routers[12]->addPort(new Port(3, 15));
    mesh->routers[12]->addPort(new Port(5, 15));
    mesh->routers[13]->addPort(new Port(3, 15));
    mesh->routers[13]->addPort(new Port(5, 15));
    mesh->routers[14]->addPort(new Port(3, 15));
    mesh->routers[14]->addPort(new Port(5, 15));
    mesh->routers[15]->addPort(new Port(3, 15));
    mesh->routers[15]->addPort(new Port(5, 15));
}

void Network::connectRingStar()
{
    QObject::connect(senders[0]->port, &Port::getPacket,
                     ringStar->routers[0], &Router::recievePacket, Qt::ConnectionType::QueuedConnection);
    QObject::connect(senders[1]->port, &Port::getPacket,
                     ringStar->routers[0], &Router::recievePacket, Qt::ConnectionType::QueuedConnection);
    QObject::connect(senders[2]->port, &Port::getPacket,
                     ringStar->routers[0], &Router::recievePacket, Qt::ConnectionType::QueuedConnection);
    QObject::connect(senders[3]->port, &Port::getPacket,
                     ringStar->routers[1], &Router::recievePacket, Qt::ConnectionType::QueuedConnection);
    QObject::connect(senders[4]->port, &Port::getPacket,
                     ringStar->routers[1], &Router::recievePacket, Qt::ConnectionType::QueuedConnection);

    QObject::connect(ringStar->routers[3]->getPortWithID(3), &Port::getPacket,
                     mesh->routers[2], &Router::recievePacket, Qt::ConnectionType::QueuedConnection);
    QObject::connect(mesh->routers[2]->getPortWithID(1), &Port::getPacket,
                     ringStar->routers[3], &Router::recievePacket, Qt::ConnectionType::QueuedConnection);
    QObject::connect(ringStar->routers[4]->getPortWithID(3), &Port::getPacket,
                     mesh->routers[1], &Router::recievePacket, Qt::ConnectionType::QueuedConnection);
    QObject::connect(mesh->routers[1]->getPortWithID(1), &Port::getPacket,
                     ringStar->routers[4], &Router::recievePacket, Qt::ConnectionType::QueuedConnection);
    QObject::connect(ringStar->routers[5]->getPortWithID(3), &Port::getPacket,
                     mesh->routers[0], &Router::recievePacket, Qt::ConnectionType::QueuedConnection);
    QObject::connect(mesh->routers[0]->getPortWithID(1), &Port::getPacket,
                     ringStar->routers[5], &Router::recievePacket, Qt::ConnectionType::QueuedConnection);

    for (auto router:ringStar->routers)
        QObject::connect(router, &Router::packetDropped,
                         this, &Network::packetDropped);
}

void Network::connectMesh()
{
    QObject::connect(receivers[0]->port, &Port::getPacket,
                     mesh->routers[12], &Router::recievePacket, Qt::ConnectionType::QueuedConnection);
    QObject::connect(mesh->routers[12]->getPortWithID(3), &Port::getPacket,
                     receivers[0], &PC::recievePacket, Qt::ConnectionType::QueuedConnection);
    QObject::connect(receivers[1]->port, &Port::getPacket,
                     mesh->routers[12], &Router::recievePacket, Qt::ConnectionType::QueuedConnection);
    QObject::connect(mesh->routers[12]->getPortWithID(5), &Port::getPacket,
                     receivers[1], &PC::recievePacket, Qt::ConnectionType::QueuedConnection);
    QObject::connect(receivers[2]->port, &Port::getPacket,
                     mesh->routers[13], &Router::recievePacket, Qt::ConnectionType::QueuedConnection);
    QObject::connect(mesh->routers[13]->getPortWithID(3), &Port::getPacket,
                     receivers[2], &PC::recievePacket, Qt::ConnectionType::QueuedConnection);
    QObject::connect(receivers[3]->port, &Port::getPacket,
                     mesh->routers[13], &Router::recievePacket, Qt::ConnectionType::QueuedConnection);
    QObject::connect(mesh->routers[13]->getPortWithID(5), &Port::getPacket,
                     receivers[3], &PC::recievePacket, Qt::ConnectionType::QueuedConnection);
    QObject::connect(receivers[4]->port, &Port::getPacket,
                     mesh->routers[14], &Router::recievePacket, Qt::ConnectionType::QueuedConnection);
    QObject::connect(mesh->routers[14]->getPortWithID(3), &Port::getPacket,
                     receivers[4], &PC::recievePacket, Qt::ConnectionType::QueuedConnection);
    QObject::connect(receivers[5]->port, &Port::getPacket,
                     mesh->routers[14], &Router::recievePacket, Qt::ConnectionType::QueuedConnection);
    QObject::connect(mesh->routers[14]->getPortWithID(5), &Port::getPacket,
                     receivers[5], &PC::recievePacket, Qt::ConnectionType::QueuedConnection);
    QObject::connect(receivers[6]->port, &Port::getPacket,
                     mesh->routers[15], &Router::recievePacket, Qt::ConnectionType::QueuedConnection);
    QObject::connect(mesh->routers[15]->getPortWithID(3), &Port::getPacket,
                     receivers[6], &PC::recievePacket, Qt::ConnectionType::QueuedConnection);
    QObject::connect(receivers[7]->port, &Port::getPacket,
                     mesh->routers[15], &Router::recievePacket, Qt::ConnectionType::QueuedConnection);
    QObject::connect(mesh->routers[15]->getPortWithID(5), &Port::getPacket,
                     receivers[7], &PC::recievePacket, Qt::ConnectionType::QueuedConnection);

    for (auto router:mesh->routers)
        QObject::connect(router, &Router::packetDropped,
                         this, &Network::packetDropped);
}

void Network::start()
{
    eventThread->start();

    for (auto receiver:receivers)
        receiver->start();
    for (auto sender:senders)
        sender->start();

    emit eventHandler->startSig();
    running = true;
}

void Network::tick(double time)
{
    if (!running) return;

    if ((int)time%MESSAGING_SYSTEM_SEND_PERIOD == 0)
    {
        QVector<QSharedPointer<Packet>> packets = messagingSystem->generatePackets();
        for (const QSharedPointer<Packet> &packet:packets)
            for (PC* sender:senders)
                if (sender->ip->ipAddr.addrToNum() == packet->getSource().ipAddr.addrToNum())
                {
                    sender->sendPacket(packet);
                    break;
                }
    }

    emit oneCycleFinished(time);
}

void Network::stop()
{
    running = false;
}
