#include <iostream>
#include <stdio.h>
#include <QVector>
#include <QThread>

#include "./headers/packet.h"
#include "./headers/router.h"

using namespace std;

QString router_sends_packet_to_another()
{
    int dummy_argc = 0;
    char x = 'b';
    char *dummy_argv[1] = {&x};
    QCoreApplication dummy(dummy_argc, dummy_argv);

    QSharedPointer<Packet> myPack = QSharedPointer<Packet>(
        new Packet("hello world", MSG_PACKET, IPV4, IPv4("255.255.255.252", "192.168.20.1"), IPv4("255.255.255.252", "20.0.0.1")));

    Router a(1, new IPv4()), b(2, new IPv4());
    a.routingTable.initFromFile("../resources/routingTables/routingTable1.csv");

    QThread bthread;
    b.moveToThread(&bthread);
    bthread.start();

    //connect port send to router rec
    QObject::connect(a.routingTable.routes[0].port, &Port::getPacket, &b, &Router::recievePacket);

    a.sendPacket(myPack);
    a.sendWaiting();
    bthread.wait(100);

    QSharedPointer<Packet> bPack = b.buffer[0];

    bthread.exit();
    bthread.wait(100);
    bthread.deleteLater();

    if (bPack->getString() != myPack->getString())
        return "string does not transfer between routers correctly";
    return "";
}

QVector<QString> run_router_tests()
{
    QVector<QString> errors;

    errors += router_sends_packet_to_another();

    return errors;
}
