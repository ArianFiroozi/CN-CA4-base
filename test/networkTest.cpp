#include <iostream>
#include <stdio.h>
#include <QVector>
#include <QThread>
#include <unistd.h>

#include "./headers/network.h"

using namespace std;

QString network_connected()
{
    int dummy_argc = 0;
    char x = 'b';
    char *dummy_argv[1] = {&x};
    QCoreApplication dummy(dummy_argc, dummy_argv);

    EventHandler* eventHandler = new EventHandler(10);
    Network net(eventHandler, OSPF, 5);

    // QSharedPointer<Packet> myPack(new Packet("hello world", MSG, IPV4, IPv4("255.255.255.255", "10.0.0.1"),
    //                                          IPv4("255.255.255.255", "192.168.20.1")));

    QObject::connect(eventHandler, &EventHandler::tick,
                     &net, &Network::tick);
    net.start();

    usleep(100000);
    // net.senders[0]->sendPacket(myPack);

    QObject::connect(net.receivers[0], &PC::packetReceived,
                     &dummy, &QCoreApplication::quit);

    dummy.exec();
    net.stop();

    if (net.receivers[0]->buffer[0]->getString() != "test")
        return "network is not connected correct!";
    return "";
}

QVector<QString> run_network_tests()
{
    QVector<QString> errors;

    errors += network_connected();

    return errors;
}
