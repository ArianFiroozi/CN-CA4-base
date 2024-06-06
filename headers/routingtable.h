#ifndef ROUTINGTABLE_H
#define ROUTINGTABLE_H

#include <QVector>
#include <iostream>

#include "ip.h"
#include "port.h"

using namespace std;

int portTranslation(int other);

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
    IPVersion ipVer;

    Route(IPv4 dest, const Mask &mask, IPv4 gateway, Port *port, int _metric = -1, RouteGate _protocol = IBGP, IPVersion _ipVer=IPV4);

    void print()
    {
        QDebug deb = qDebug();
        if (ipVer==IPV6)
            deb << "\t" << IPv6::mapFromIPv4(dest).getIPStr().toStdString() << "via" <<
                IPv6::mapFromIPv4(gateway).getIPStr().toStdString();
        else
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
    RoutingTable(IPv4 *_masterIP = new IPv4());

    QVector<Route> routes;
    IPv4 *masterIP;

    void addRoute(Route newRoute);
    QVector<Route> findAllRoutes(IPv4 ip);
    Route findBestRoute(IPv4 ip); //based on metric

    void print();
    void initFromFile(QString address);
    void initFromFile(QString address, Port* port);
    bool updateFromPacketRIP(QString msg, Port* port, int time, IPVersion ipVer);
    QString toString(IPv4 gateway, int portID);
    bool updateFromPacketOSPF(QString msg, Port* port, IPVersion ipVer);
    void removeTimeOutRoutes(int time);
};

#endif // ROUTINGTABLE_H
