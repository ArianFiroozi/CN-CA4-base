#ifndef ROUTINGTABLE_H
#define ROUTINGTABLE_H

#include <QVector>
#include <iostream>

#include "ip.h"
#include "port.h"

using namespace std;

enum RouteGate
{
    IBGP,
    EBGP
};

struct Route
{
    IPv4 dest;
    Mask mask;
    IPv4 gateway;
    Port* port;
    int metric; // min is best, RIP: hops, OSPF: delay
    int timeOut;
    QVector<int> asIDs;
    RouteGate protocol;

    Route(IPv4 dest, const Mask &mask, IPv4 gateway, Port *port, int _metric = -1, RouteGate _protocol = IBGP);

    void print()
    {
        QDebug deb = qDebug();
        deb << "\t" << dest.getIPStr().toStdString() << "via" << gateway.getIPStr().toStdString();
        deb << "metric =" << metric;
        deb << ((protocol == IBGP)? "iBGP" : "eBGP");
        deb << "AS:";
        for (auto i:asIDs)
            deb << i;

    }
};

class RoutingTable
{
public:
    RoutingTable(IPv4 *_masterIP);

    QVector<Route> routes;
    IPv4 *masterIP;

    void addRoute(Route newRoute);
    QVector<Route> findAllRoutes(IPv4 ip);
    Route findBestRoute(IPv4 ip); //based on metric

    void print();
    void initFromFile(QString address);
    void initFromFile(QString address, Port* port);
    bool updateFromPacketRIP(QString msg, Port* port, int time);
    QString toStringRIP(IPv4 gateway);
    bool updateFromPacketOSPF(QString msg, Port* port);
    QString toStringOSPF(IPv4 gateway);
    void removeTimeOutRoutes(int time);
};

#endif // ROUTINGTABLE_H
