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
        return Route(IPv4("255.255.255.255", "255.255.255.255"),
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
        IPv4 dest(lineVec[2], lineVec[1]);
        Mask mask;
        mask.strToMask(lineVec[2]);
        IPv4 gateway(lineVec[2], lineVec[3]);
        int portID(lineVec[4].toInt());

        Route newRoute(dest, mask, gateway, new Port(portID));
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
        IPv4 dest(lineVec[2], lineVec[1]);
        Mask mask;
        mask.strToMask(lineVec[2]);
        IPv4 gateway(lineVec[2], lineVec[3]);
        int portID(lineVec[4].toInt());
        if (port->id == portID)
        {
            Route newRoute(dest, mask, gateway, port);
            this->routes.append(newRoute);
        }
    }
}

void RoutingTable::updateFromPacketRIP(QString msg, Port* port)
{
    QVector<QString> routesStr = msg.split("#");
    QString gatewayStr = routesStr[routesStr.length() - 1];
    for (QString routeStr : routesStr)
    {
        if (routeStr == gatewayStr)
            break;

        int metric;
        IPAddr ipAddr;
        Mask mask;

        ipAddr.strToIP(routeStr.split(",")[0]);
        mask.strToMask(routeStr.split(",")[1]);
        metric = routeStr.split(",")[2].toInt() + 1;

        bool betterRouteExists = false;
        Route newRoute = Route(IPv4(mask.toStr(), ipAddr.toStr()), mask, gatewayStr, port, metric);
        for (int i=0; i<routes.length();i++)
        {
            if (routes[i].dest.ipAddr.addrToNum() == newRoute.dest.ipAddr.addrToNum())
            {
                if (routes[i].metric <= newRoute.metric)
                    betterRouteExists = true;
                else
                    routes.remove(i);
                break;
            }
        }
        if (!betterRouteExists)
            routes.append(newRoute);
    }
}

QString RoutingTable::toStringRIP(IPv4 gateway)
{
    QString ripStr;

    for (Route route : routes)
    {
        ripStr.append(route.dest.ipAddr.toStr());
        ripStr.append(",");
        ripStr.append(route.dest.mask.toStr());
        ripStr.append(",");
        ripStr.append(QString::number(route.metric));
        ripStr.append("#");
    }
    ripStr.append(gateway.ipAddr.toStr());
    return ripStr;
}

Route::Route(IPv4 dest, const Mask &mask, IPv4 gateway, Port *port, int _metric) :
    dest(dest),
    mask(mask),
    gateway(gateway),
    port(port)
{
    metric = _metric;
}
