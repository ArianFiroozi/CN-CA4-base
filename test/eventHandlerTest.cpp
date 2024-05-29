#include <iostream>
#include <stdio.h>
#include <QVector>
#include <QThread>
#include <unistd.h>

#include "./headers/eventHandler.h"
#include "./headers/cluster.h"
#include "./headers/pc.h"

using namespace std;

QString event_handler_with_mesh_cluster_static_init_correct()
{
    int dummy_argc = 0;
    char x = 'b';
    char *dummy_argv[1] = {&x};
    QCoreApplication dummy(dummy_argc, dummy_argv);

    Mesh cluster(4, 4, IPv4("255.255.255.255", "20.0.0.0"), MANUAL);
    QSharedPointer<Packet> myPack = QSharedPointer<Packet>(new Packet("hello world", MSG, IPV4, IPv4("255.255.255.255", "20.0.0.1"),
                                                                      IPv4("255.255.255.255", "192.168.20.4")));
    EventHandler* eventHandler = new EventHandler(1);
    QThread* eventThread = new QThread();
    eventHandler->moveToThread(eventThread);

    PC sender(1, new IPv4("255.255.255.255", "192.168.20.1"), new Port(1));
    PC receiver(2, new IPv4("255.255.255.255", "192.168.20.4"), new Port(2));

    cluster.routers.last()->addPort(new Port(3));
    QString path("../resources/routingTables/manualMesh4x4/routingTable16.csv");
    cluster.routers.last()->routingTable.initFromFile(path, cluster.routers.last()->getPortWithID(3));

    QObject::connect(sender.port, &Port::getPacket,
                     cluster.routers[0], &Router::recievePacket);
    QObject::connect(cluster.routers.last()->getPortWithID(3), &Port::getPacket,
                     &receiver, &PC::recievePacket);

    cluster.connectTick(eventHandler);
    eventThread->start();

    sender.sendPacket(myPack);
    emit eventHandler->startSig();

    QObject::connect(&receiver, &PC::packetReceived,
                     &dummy, &QCoreApplication::quit);

    dummy.exec();

    if (receiver.buffer[0]->getString() != "hello world")
        return "message did not reach other pc in mesh!";
    return "";
}

QVector<QString> run_event_handler_tests()
{
    QVector<QString> errors;

    errors += event_handler_with_mesh_cluster_static_init_correct();

    return errors;
}
