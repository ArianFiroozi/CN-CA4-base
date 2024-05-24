#include "./headers/routingtable.h"

#include <QFile>

RoutingTable::RoutingTable()
{}

void RoutingTable::addRoute(Route newRoute)
{
    routes.append(newRoute);
}

QVector<Route> RoutingTable::findAllRoutes(IPv4 ip)
{
    QVector<Route> compatibleRoutes;
    for (Route route : routes)
        if (route.dest.includes(ip))
            compatibleRoutes.append(route);
    return compatibleRoutes;
}

Route RoutingTable::findBestRoute(IPv4 ip)
{
    if (!routes.size())
        return Route(MANUAL, IPv4("255.255.255.255", "255.255.255.255"),
                     Mask(), IPv4("255.255.255.255", "255.255.255.255"),
                     new Port(0));

    QVector<Route> maxRoutes = findAllRoutes(ip);
    int maxMetric = -1;
    for (Route route : maxRoutes)
        if (route.metric > maxMetric)
            maxMetric = route.metric;

    for (Route route : maxRoutes)
        if (route.metric == maxMetric)
            return route;

    return routes[0];
}

void RoutingTable::print()
{
    for (Route route : routes)
        route.print();
}

void RoutingTable::initFromFile(QString address)
{
    QFile file(address);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&file);
    while (!in.atEnd())
    {
        QString line = in.readLine();

        if(line.startsWith("//")) continue;

        QVector<QString> lineVec = line.split(",");
        RoutingProtocol protocol = (lineVec[0] == "OSPF") ? OSPF : BGP;
        IPv4 dest(lineVec[2], lineVec[1]);
        Mask mask;
        mask.strToMask(lineVec[2]);
        IPv4 gateway(lineVec[2], lineVec[3]);
        int portID(lineVec[4].toInt());

        Route newRoute(protocol, dest, mask, gateway, new Port(portID));
        this->routes.append(newRoute);
    }
}

void RoutingTable::initFromFile(QString address, Port* port)
{
    QFile file(address);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&file);
    while (!in.atEnd())
    {
        QString line = in.readLine();

        if(line.startsWith("//")) continue;

        QVector<QString> lineVec = line.split(",");
        RoutingProtocol protocol = (lineVec[0] == "OSPF") ? OSPF : ((lineVec[0] == "MANUAL") ? MANUAL : BGP);
        IPv4 dest(lineVec[2], lineVec[1]);
        Mask mask;
        mask.strToMask(lineVec[2]);
        IPv4 gateway(lineVec[2], lineVec[3]);
        int portID(lineVec[4].toInt());
        if (port->id == portID)
        {
            Route newRoute(protocol, dest, mask, gateway, port);
            this->routes.append(newRoute);
        }
    }
}

Route::Route(RoutingProtocol protocol, IPv4 dest, const Mask &mask, IPv4 gateway, Port *port) : protocol(protocol),
    dest(dest),
    mask(mask),
    gateway(gateway),
    port(port)
{
    metric = -1;
}
