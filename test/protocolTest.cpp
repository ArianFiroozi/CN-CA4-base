#include <iostream>
#include <stdio.h>
#include <QVector>
#include <QThread>
#include <unistd.h>

#include "./headers/eventHandler.h"
#include "./headers/cluster.h"
#include "./headers/pc.h"

using namespace std;

QString simple_rip_on_mesh()
{
    int dummy_argc = 0;
    char x = 'b';
    char *dummy_argv[1] = {&x};
    QCoreApplication dummy(dummy_argc, dummy_argv);

    Mesh cluster(4, 4, IPv4("255.255.255.255", "20.0.0.0"), RIP);
    QSharedPointer<Packet> myPack = QSharedPointer<Packet>(new Packet("hello world", MSG, IPV4, IPv4("255.255.255.255", "20.0.0.1"),
                                                                      IPv4("255.255.255.255", "192.168.20.4")));
    EventHandler* eventHandler = new EventHandler(10);
    QThread* eventThread = new QThread();
    eventHandler->moveToThread(eventThread);

    PC sender(1, new IPv4("255.255.255.255", "192.168.20.1"), new Port(3));
    PC receiver(2, new IPv4("255.255.255.255", "192.168.20.4"), new Port(1));

    cluster.routers.last()->addPort(new Port(3));

    QObject::connect(sender.port, &Port::getPacket,
                     cluster.routers[0], &Router::recievePacket, Qt::ConnectionType::QueuedConnection);
    QObject::connect(receiver.port, &Port::getPacket,
                     cluster.routers.last(), &Router::recievePacket, Qt::ConnectionType::QueuedConnection);
    QObject::connect(cluster.routers.last()->getPortWithID(3), &Port::getPacket,
                     &receiver, &PC::recievePacket, Qt::ConnectionType::QueuedConnection);

    cluster.connectTick(eventHandler);
    eventThread->start();
    receiver.start();
    sender.start();

    emit eventHandler->startSig();

    QObject::connect(&receiver, &PC::packetReceived,
                     &dummy, &QCoreApplication::quit);

    usleep(10000);

    sender.sendPacket(myPack);

    dummy.exec();

    // cluster.printRoutingTables();

    if (receiver.buffer[0]->getString() != "hello world")
        return "RIP simple connection wrong!";
    return "";
}

QString simple_ospf_on_mesh_with_delay()
{
    int dummy_argc = 0;
    char x = 'b';
    char *dummy_argv[1] = {&x};
    QCoreApplication dummy(dummy_argc, dummy_argv);

    Mesh cluster(4, 4, IPv4("255.255.255.255", "20.0.0.0"), OSPF, true);
    QSharedPointer<Packet> myPack = QSharedPointer<Packet>(new Packet("hello world", MSG, IPV4, IPv4("255.255.255.255", "20.0.0.1"),
                                                                      IPv4("255.255.255.255", "192.168.20.4")));
    EventHandler* eventHandler = new EventHandler(10);
    QThread* eventThread = new QThread();
    eventHandler->moveToThread(eventThread);

    PC sender(1, new IPv4("255.255.255.255", "192.168.20.1"), new Port(3));
    PC receiver(2, new IPv4("255.255.255.255", "192.168.20.4"), new Port(1));

    cluster.routers.last()->addPort(new Port(3, 15));

    QObject::connect(sender.port, &Port::getPacket,
                     cluster.routers[0], &Router::recievePacket, Qt::ConnectionType::QueuedConnection);
    QObject::connect(receiver.port, &Port::getPacket,
                     cluster.routers.last(), &Router::recievePacket, Qt::ConnectionType::QueuedConnection);
    QObject::connect(cluster.routers.last()->getPortWithID(3), &Port::getPacket,
                     &receiver, &PC::recievePacket, Qt::ConnectionType::QueuedConnection);

    cluster.connectTick(eventHandler);
    eventThread->start();
    receiver.start();
    sender.start();

    emit eventHandler->startSig();

    QObject::connect(&receiver, &PC::packetReceived,
                     &dummy, &QCoreApplication::quit);

    usleep(100000);

    sender.sendPacket(myPack);

    // cluster.printRoutingTables();
    dummy.exec();


    if (receiver.buffer[0]->getString() != "hello world")
        return "RIP simple connection wrong!";
    return "";
}

QString simple_rip_on_ring_star()
{
    int dummy_argc = 0;
    char x = 'b';
    char *dummy_argv[1] = {&x};
    QCoreApplication dummy(dummy_argc, dummy_argv);

    RingStar cluster(7, {2, 4, 6, 7}, IPv4("255.255.255.255", "20.0.0.0"), RIP);
    QSharedPointer<Packet> myPack(new Packet("hello world", MSG, IPV4, IPv4("255.255.255.255", "20.0.0.1"),
                                             IPv4("255.255.255.255", "192.168.20.2")));

    EventHandler* eventHandler = new EventHandler(10);
    QThread* eventThread = new QThread();
    eventHandler->moveToThread(eventThread);

    PC sender(1, new IPv4("255.255.255.255", "192.168.10.1"), new Port(3));
    PC receiver(2, new IPv4("255.255.255.255", "192.168.20.2"), new Port(3));

    cluster.routers[4]->addPort(new Port(1));

    QObject::connect(sender.port, &Port::getPacket,
                     cluster.routers[1], &Router::recievePacket);
    QObject::connect(cluster.routers[4]->getPortWithID(1), &Port::getPacket,
                     &receiver, &PC::recievePacket);
    QObject::connect(receiver.port, &Port::getPacket,
                     cluster.routers[4], &Router::recievePacket, Qt::ConnectionType::QueuedConnection);

    cluster.connectTick(eventHandler);
    eventThread->start();
    receiver.start();
    sender.start();

    emit eventHandler->startSig();

    QObject::connect(&receiver, &PC::packetReceived,
                     &dummy, &QCoreApplication::quit);

    usleep(10000);

    sender.sendPacket(myPack);

    dummy.exec();

    cluster.printRoutingTables();

    if (receiver.buffer[0]->getString() != "hello world")
        return "RIP simple connection wrong!";
    return "";
}

QString simple_ospf_on_ring_star_with_delay()
{
    int dummy_argc = 0;
    char x = 'b';
    char *dummy_argv[1] = {&x};
    QCoreApplication dummy(dummy_argc, dummy_argv);

    RingStar cluster(7, {2, 4, 6, 7}, IPv4("255.255.255.255", "20.0.0.0"), OSPF, true);
    QSharedPointer<Packet> myPack(new Packet("hello world", MSG, IPV4, IPv4("255.255.255.255", "20.0.0.1"),
                                             IPv4("255.255.255.255", "192.168.20.2")));

    EventHandler* eventHandler = new EventHandler(10);
    QThread* eventThread = new QThread();
    eventHandler->moveToThread(eventThread);

    PC sender(1, new IPv4("255.255.255.255", "192.168.10.1"), new Port(3, 10));
    PC receiver(2, new IPv4("255.255.255.255", "192.168.20.2"), new Port(3, 10));

    cluster.routers[4]->addPort(new Port(1, 10));

    QObject::connect(sender.port, &Port::getPacket,
                     cluster.routers[0], &Router::recievePacket);

    QObject::connect(cluster.routers[4]->getPortWithID(1), &Port::getPacket,
                     &receiver, &PC::recievePacket);

    QObject::connect(receiver.port, &Port::getPacket,
                     cluster.routers[4], &Router::recievePacket, Qt::ConnectionType::QueuedConnection);

    cluster.connectTick(eventHandler);
    eventThread->start();
    receiver.start();
    sender.start();

    emit eventHandler->startSig();

    QObject::connect(&receiver, &PC::packetReceived,
                     &dummy, &QCoreApplication::quit);

    usleep(100000);

    sender.sendPacket(myPack);

    // cluster.printRoutingTables();
    dummy.exec();

    if (receiver.buffer[0]->getString() != "hello world")
        return "OSPF simple connection wrong!";
    return "";
}


QVector<QString> run_protocol_tests()
{
    QVector<QString> errors;

    errors += simple_rip_on_mesh();
    errors += simple_rip_on_ring_star();
    errors += simple_ospf_on_mesh_with_delay();
    errors += simple_ospf_on_ring_star_with_delay();

    return errors;
}
