#include "../headers/network.h"

Network::Network(EventHandler* _eventHandler, RoutingProtocol protocol)
{
    eventHandler = _eventHandler;

    ringStar = new RingStar(7, {2, 4, 6, 7}, IPv4("255.255.255.0", "10.0.0.0"), protocol, true);
    mesh = new Mesh(4, 4, IPv4("255.255.255.0", "20.0.0.0"), protocol, true);

    eventThread = new QThread();
    eventHandler->moveToThread(eventThread);

    senders.append(new PC(1, new IPv4("255.255.255.255", "192.168.10.1"), new Port(3, 10)));

    receivers.append(new PC(2, new IPv4("255.255.255.255", "192.168.20.2"), new Port(1, 10)));

    mesh->routers[1]->addPort(new Port(1, 10));

    QObject::connect(senders[0]->port, &Port::getPacket,
                     ringStar->routers[0], &Router::recievePacket, Qt::ConnectionType::QueuedConnection);

    QObject::connect(ringStar->routers[4]->getPortWithID(3), &Port::getPacket,
                     mesh->routers[1], &Router::recievePacket, Qt::ConnectionType::QueuedConnection);
    QObject::connect(mesh->routers[1]->getPortWithID(1), &Port::getPacket,
                     ringStar->routers[4], &Router::recievePacket, Qt::ConnectionType::QueuedConnection);

    mesh->routers.last()->addPort(new Port(3, 15));

    QObject::connect(receivers[0]->port, &Port::getPacket,
                     mesh->routers.last(), &Router::recievePacket, Qt::ConnectionType::QueuedConnection);
    QObject::connect(mesh->routers.last()->getPortWithID(3), &Port::getPacket,
                     receivers[0], &PC::recievePacket, Qt::ConnectionType::QueuedConnection);

    ringStar->connectTick(eventHandler);
    mesh->connectTick(eventHandler);
}

void Network::start()
{
    eventThread->start();

    for (auto receiver:receivers)
        receiver->start();
    for (auto sender:senders)
        sender->start();

    emit eventHandler->startSig();
}
