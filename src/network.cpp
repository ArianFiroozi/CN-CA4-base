#include "../headers/network.h"

Network::Network(EventHandler* _eventHandler, RoutingProtocol protocol, int lambda)
{
    eventHandler = _eventHandler;
    running = false;

    ringStar = new RingStar(7, {2, 4, 6, 7}, IPv4("255.255.255.0", "10.0.0.0"), protocol, true,
                            new DhcpServer(new IPv4("255.255.255.255", "0.0.0.0"), new IPv4("255.255.255.255", "192.168.10.0"),
                                           new IPv4("255.255.255.255", "192.168.10.250"), new Port(555, 10)));
    mesh = new Mesh(4, 4, IPv4("255.255.255.0", "20.0.0.0"), protocol, true,
                    new DhcpServer(new IPv4("255.255.255.255", "0.0.0.0"), new IPv4("255.255.255.255", "192.168.20.0"),
                                   new IPv4("255.255.255.255", "192.168.20.250"), new Port(555, 10)));
    torus = new Torus(3,3,IPv4("255.255.255.0", "30.0.0.0"), protocol, true,
                      new DhcpServer(new IPv4("255.255.255.255", "0.0.0.0"), new IPv4("255.255.255.255", "192.168.30.0"),
                                     new IPv4("255.255.255.255", "192.168.30.250"), new Port(555, 10)));

    eventThread = new QThread();
    eventHandler->moveToThread(eventThread);

    createSenders();
    createReceivers();
    addRingStarPorts();
    addMeshPorts();
    addTorusPorts();
    connectRingStar();
    connectMesh();
    connectTorus();

    ringStar->connectTick(eventHandler);
    mesh->connectTick(eventHandler);
    torus->connectTick(eventHandler);
    connectPcTick();

    messagingSystem = new MessagingSystem(lambda, receivers, senders);
    packetsSent = packetsReceived = totalWaitCycles = totalQueueWaitCycles = packetsDropped = 0;
    highestQueueWait = highestWait = -1;
    leastWait = leastQueueWait = 100000;
}

Network::~Network()
{
    delete mesh;
    delete ringStar;
    delete torus;

    for (auto receiver:receivers)
        delete receiver;
    for (auto sender:senders)
        delete sender;
    // delete eventHandler;

    eventThread->quit();
    eventThread->deleteLater();
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
    qDebug() << "";
    qDebug() << "----------------------MESH------------------------";
    mesh->printRoutingTables();
    qDebug() << "";
    qDebug() << "--------------------RINGSTAR----------------------";
    ringStar->printRoutingTables();
    qDebug() << "";
    qDebug() << "----------------------TORUS-----------------------";
    torus->printRoutingTables();
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

void Network::connectPcTick()
{
    for (auto receiver:receivers)
        receiver->connectTick(eventHandler);
    for (auto sender:senders)
        sender->connectTick(eventHandler);
}

void Network::createSenders()
{
    senders.append(new PC(1, new Port(21, 10)));
    senders.append(new PC(2, new Port(22, 10)));
    senders.append(new PC(3, new Port(23, 10)));
    senders.append(new PC(4, new Port(21, 10)));
    senders.append(new PC(5, new Port(22, 10)));
    senders.append(new PC(6, new Port(31, 10), IPV6));
    senders.append(new PC(7, new Port(32, 10), IPV6));
    senders.append(new PC(8, new Port(33, 10), IPV6));


    for (auto sender: senders)
        QObject::connect(sender, &PC::packetSent,
                         this, &Network::packetSent, Qt::ConnectionType::QueuedConnection);
}

void Network::createReceivers()
{
    receivers.append(new PC(1, new Port(1, 10), IPV6));
    receivers.append(new PC(2, new Port(5, 10), IPV6));
    receivers.append(new PC(3, new Port(1, 10), IPV6));
    receivers.append(new PC(4, new Port(5, 10), IPV6));
    receivers.append(new PC(5, new Port(1, 10), IPV6));
    receivers.append(new PC(6, new Port(5, 10), IPV6));
    receivers.append(new PC(7, new Port(1, 10), IPV6));
    receivers.append(new PC(8, new Port(5, 10), IPV6));

    for (auto receiver: receivers)
        QObject::connect(receiver, &PC::packetReceived,
                         this, &Network::packetReceived, Qt::ConnectionType::QueuedConnection);
}

void Network::addRingStarPorts()
{
    //senders
    ringStar->routers[0]->addPort(new Port(21, 10));
    ringStar->routers[0]->addPort(new Port(22, 10));
    ringStar->routers[0]->addPort(new Port(23, 10));
    ringStar->routers[1]->addPort(new Port(21, 10));
    ringStar->routers[1]->addPort(new Port(22, 10));

    // mesh
    ringStar->routers[3]->addPort(new Port(3, 10));
    ringStar->routers[4]->addPort(new Port(3, 10));
    ringStar->routers[5]->addPort(new Port(3, 10));

    // torus
    ringStar->routers[0]->addPort(new Port(42, 10));
    ringStar->routers[6]->addPort(new Port(42, 10));
}

void Network::addMeshPorts()
{
    // ringstar
    mesh->routers[0]->addPort(new Port(1, 10));
    mesh->routers[1]->addPort(new Port(1, 10));
    mesh->routers[2]->addPort(new Port(1, 10));

    // receivers
    mesh->routers[12]->addPort(new Port(3, 15));
    mesh->routers[12]->addPort(new Port(5, 15));
    mesh->routers[13]->addPort(new Port(3, 15));
    mesh->routers[13]->addPort(new Port(5, 15));
    mesh->routers[14]->addPort(new Port(3, 15));
    mesh->routers[14]->addPort(new Port(5, 15));
    mesh->routers[15]->addPort(new Port(3, 15));
    mesh->routers[15]->addPort(new Port(5, 15));
}

void Network::addTorusPorts()
{
    // senders
    torus->routers[6]->addPort(new Port(31, 10));
    torus->routers[7]->addPort(new Port(32, 10));
    torus->routers[8]->addPort(new Port(33, 10));

    // ring star
    torus->routers[2]->addPort(new Port(42, 10));
    torus->routers[5]->addPort(new Port(42, 10));
}

void Network::connectRingStar()
{
    QObject::connect(senders[0]->port, &Port::getPacket,
                     ringStar->routers[0], &Router::recievePacket, Qt::ConnectionType::QueuedConnection);
    QObject::connect(ringStar->routers[0]->getPortWithID(21), &Port::getPacket,
                     senders[0], &PC::recievePacket, Qt::ConnectionType::QueuedConnection);
    QObject::connect(senders[1]->port, &Port::getPacket,
                     ringStar->routers[0], &Router::recievePacket, Qt::ConnectionType::QueuedConnection);
    QObject::connect(ringStar->routers[0]->getPortWithID(22), &Port::getPacket,
                     senders[1], &PC::recievePacket, Qt::ConnectionType::QueuedConnection);
    QObject::connect(senders[2]->port, &Port::getPacket,
                     ringStar->routers[0], &Router::recievePacket, Qt::ConnectionType::QueuedConnection);
    QObject::connect(ringStar->routers[0]->getPortWithID(23), &Port::getPacket,
                     senders[2], &PC::recievePacket, Qt::ConnectionType::QueuedConnection);

    QObject::connect(senders[3]->port, &Port::getPacket,
                     ringStar->routers[1], &Router::recievePacket, Qt::ConnectionType::QueuedConnection);
    QObject::connect(ringStar->routers[1]->getPortWithID(21), &Port::getPacket,
                     senders[3], &PC::recievePacket, Qt::ConnectionType::QueuedConnection);
    QObject::connect(senders[4]->port, &Port::getPacket,
                     ringStar->routers[1], &Router::recievePacket, Qt::ConnectionType::QueuedConnection);
    QObject::connect(ringStar->routers[1]->getPortWithID(22), &Port::getPacket,
                     senders[4], &PC::recievePacket, Qt::ConnectionType::QueuedConnection);

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

void Network::connectTorus()
{
    QObject::connect(torus->routers[6]->getPortWithID(31), &Port::getPacket,
                     senders[5], &PC::recievePacket, Qt::ConnectionType::QueuedConnection);
    QObject::connect(senders[5]->port, &Port::getPacket,
                     torus->routers[6], &Router::recievePacket, Qt::ConnectionType::QueuedConnection);
    QObject::connect(torus->routers[7]->getPortWithID(32), &Port::getPacket,
                     senders[6], &PC::recievePacket, Qt::ConnectionType::QueuedConnection);
    QObject::connect(senders[6]->port, &Port::getPacket,
                     torus->routers[7], &Router::recievePacket, Qt::ConnectionType::QueuedConnection);
    QObject::connect(torus->routers[8]->getPortWithID(33), &Port::getPacket,
                     senders[7], &PC::recievePacket, Qt::ConnectionType::QueuedConnection);
    QObject::connect(senders[7]->port, &Port::getPacket,
                     torus->routers[8], &Router::recievePacket, Qt::ConnectionType::QueuedConnection);

    QObject::connect(torus->routers[2]->getPortWithID(42), &Port::getPacket,
                     ringStar->routers[0], &Router::recievePacket, Qt::ConnectionType::QueuedConnection);
    QObject::connect(ringStar->routers[0]->getPortWithID(42), &Port::getPacket,
                     torus->routers[2], &Router::recievePacket, Qt::ConnectionType::QueuedConnection);
    QObject::connect(torus->routers[5]->getPortWithID(42), &Port::getPacket,
                     ringStar->routers[6], &Router::recievePacket, Qt::ConnectionType::QueuedConnection);
    QObject::connect(ringStar->routers[6]->getPortWithID(42), &Port::getPacket,
                     torus->routers[5], &Router::recievePacket, Qt::ConnectionType::QueuedConnection);

    for (auto router:torus->routers)
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
