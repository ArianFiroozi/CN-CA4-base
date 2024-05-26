#include <iostream>
#include <stdio.h>
#include <QVector>
#include <QThread>
#include <unistd.h>

#include "./headers/cluster.h"
#include "./headers/pc.h"

using namespace std;

QString mesh_cluster_static_init_correct()
{
    Mesh cluster(4, 4, IPv4("255.255.255.255", "20.0.0.0"));
    // cluster.printRoutingTables();

    Packet myPack("hello world", MSG, IPV4, IPv4("255.255.255.255", "20.0.0.1"),
                  IPv4("255.255.255.255", "192.168.20.4"));

    PC sender(1, new IPv4("255.255.255.255", "192.168.20.1"), new Port(1));
    PC receiver(2, new IPv4("255.255.255.255", "192.168.20.4"), new Port(2));

    cluster.routers.last()->addPort(new Port(3));
    QString path("../resources/routingTables/manualMesh4x4/routingTable16.csv");
    cluster.routers.last()->routingTable.initFromFile(path, cluster.routers.last()->getPortWithID(3));

    QObject::connect(sender.port, &Port::getPacket,
                     cluster.routers[0], &Router::recievePacket);
    QObject::connect(cluster.routers.last()->getPortWithID(3), &Port::getPacket,
                     &receiver, &PC::recievePacket);

    sender.sendPacket(myPack);

    for (auto router:cluster.routers)
    {
        usleep(10);
        router->forward();
    }

    if (receiver.buffer[0].getString() != "hello world")
        return "message did not reach other pc in mesh!";
    return "";
}

QString ring_cluster_static_init_correct()
{
    RingStar cluster(7, {2, 4, 6, 7}, IPv4("255.255.255.255", "20.0.0.0"));
    // cluster.printRoutingTables();

    Packet myPack("hello world", MSG, IPV4, IPv4("255.255.255.255", "20.0.0.1"),
                  IPv4("255.255.255.255", "192.168.10.2"));

    PC sender(1, new IPv4("255.255.255.255", "192.168.10.1"), new Port(1));
    PC receiver(2, new IPv4("255.255.255.255", "192.168.10.2"), new Port(2));

    QObject::connect(sender.port, &Port::getPacket,
                     cluster.routers[1], &Router::recievePacket);
    QObject::connect(cluster.routers[4]->getPortWithID(3), &Port::getPacket,
                     &receiver, &PC::recievePacket);

    sender.sendPacket(myPack);

    for (auto router:cluster.routers)
    {
        usleep(10);
        router->forward();
    }

    if (receiver.buffer[0].getString() != "hello world")
        return "message did not reach other pc in mesh!";
    return "";
}

QString star_cluster_static_init_correct()
{
    RingStar cluster(7, {2, 4, 6, 7}, IPv4("255.255.255.255", "20.0.0.0"));
    // cluster.printRoutingTables();

    Packet myPack("hello world", MSG, IPV4, IPv4("255.255.255.255", "20.0.0.1"),
                  IPv4("255.255.255.255", "192.168.10.2"));

    PC sender(1, new IPv4("255.255.255.255", "192.168.10.1"), new Port(1));
    PC receiver(2, new IPv4("255.255.255.255", "192.168.10.2"), new Port(2));

    QObject::connect(sender.port, &Port::getPacket,
                     cluster.routers[7], &Router::recievePacket);
    QObject::connect(cluster.routers[4]->getPortWithID(3), &Port::getPacket,
                     &receiver, &PC::recievePacket);

    sender.sendPacket(myPack);

    for (auto router:cluster.routers)
    {
        usleep(10);
        router->forward();
    }

    for (auto router:cluster.routers)
    {
        usleep(10);
        router->forward();
    }

    if (receiver.buffer[0].getString() != "hello world")
        return "message did not reach other pc in mesh!";
    return "";
}

QVector<QString> run_cluster_tests()
{
    QVector<QString> errors;

    errors += mesh_cluster_static_init_correct();
    errors += ring_cluster_static_init_correct();
    errors += star_cluster_static_init_correct();

    return errors;
}
