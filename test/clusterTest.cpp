#include <iostream>
#include <stdio.h>
#include <QVector>
#include <QThread>

#include "./headers/cluster.h"

using namespace std;

QString mesh_cluster_static_init_correct()
{
    Mesh cluster(4, 4, IPv4("255.255.255.252", "20.0.0.0"));
    // cluster.printRoutingTables();

    Packet myPack("hello world", MSG, IPV4, IPv4("255.255.255.252", "20.0.0.1"),
                  IPv4("255.255.255.252", "192.168.20.3"));
    cluster.getRouter(5)->sendPacket(myPack);

    for (auto router:cluster.routers)
        router->forward();

    // cout << cluster.getRouter(13)->buffer[0].toStr().toStdString() << endl;
    return "";
}

QVector<QString> run_cluster_tests()
{
    QVector<QString> errors;

    errors += mesh_cluster_static_init_correct();

    return errors;
}
