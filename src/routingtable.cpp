#include "./headers/routingtable.h"
#include "../headers/common.h"

#include <QFile>

RoutingTable::RoutingTable(IPv4 *_masterIP)
{
    masterIP = _masterIP;
}

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

    QVector<Route> minRoutes = findAllRoutes(ip);
    int minMetric = 10000000000;
    for (Route route : minRoutes)
        if (route.metric < minMetric)
            minMetric = route.metric;

    for (Route route : minRoutes)
        if (route.metric == minMetric)
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

bool RoutingTable::updateFromPacketRIP(QString msg, Port* port, int time)
{
    int updated = false;
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

        if (metric > RIP_MAX_HOP)
            return false;

        bool betterRouteExists = false;
        Route newRoute = Route(IPv4(mask.toStr(), ipAddr.toStr()), mask, IPv4(mask.toStr(), gatewayStr), port, metric);
        for (QString as: routeStr.split(",")[3].split("-"))
            if (as != "")
                newRoute.asIDs.append(as.toInt());
        newRoute.timeOut = time + RIP_TIMEOUT;
        if (newRoute.asIDs.size() == 0)
            newRoute.asIDs.append(this->masterIP->ipAddr.netID1);

        if (masterIP->ipAddr.netID1 != newRoute.asIDs.last()){
            if (newRoute.asIDs.contains(this->masterIP->ipAddr.netID1))
                return updated;

            newRoute.asIDs.append(this->masterIP->ipAddr.netID1);
            newRoute.metric = newRoute.asIDs.size();
            newRoute.protocol = EBGP;
        }

        if (masterIP->includes(newRoute.gateway))
            if (newRoute.protocol == EBGP) return updated;

        for (int i=0; i<routes.length();i++)
        {
            if (routes[i].dest.ipAddr.addrToNum() == newRoute.dest.ipAddr.addrToNum() &&
                newRoute.gateway.ipAddr.addrToNum() != masterIP->ipAddr.addrToNum())
            {
                if (routes[i].metric < newRoute.metric)
                    betterRouteExists = true;
                else if (routes[i].metric == newRoute.metric)
                {
                    betterRouteExists = true;
                    routes.remove(i);
                    addRoute(newRoute); // keep alive
                }
                else
                    routes.remove(i);
                break;
            }
        }
        if (!betterRouteExists)
        {
            addRoute(newRoute);
            updated = true;
        }
    }
    return updated;
}

void RoutingTable::removeTimeOutRoutes(int time)
{
    for (int i=routes.length() - 1; i >= 0;i--)
        if (routes[i].timeOut < time)
            routes.remove(i);
}

bool RoutingTable::updateFromPacketOSPF(QString msg, Port* port)
{
    int updated = false;
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
        metric = routeStr.split(",")[2].toInt() + port->delay;

        bool betterRouteExists = false;
        Route newRoute = Route(IPv4(mask.toStr(), ipAddr.toStr()), mask, IPv4(mask.toStr(), gatewayStr), port, metric);
        if (newRoute.asIDs.size() == 0)
            newRoute.asIDs.append(this->masterIP->ipAddr.netID1);

        if (masterIP->ipAddr.netID1 != newRoute.asIDs.last()){
            if (newRoute.asIDs.contains(this->masterIP->ipAddr.netID1))
                return updated;

            newRoute.asIDs.append(this->masterIP->ipAddr.netID1);
            newRoute.metric = newRoute.asIDs.size();
            newRoute.protocol = EBGP;
        }

        if (masterIP->includes(newRoute.gateway))
            if (newRoute.protocol == EBGP) return updated;

        for (int i=0; i<routes.length();i++)
        {
            if (routes[i].dest.ipAddr.addrToNum() == newRoute.dest.ipAddr.addrToNum() &&
                newRoute.gateway.ipAddr.addrToNum() != masterIP->ipAddr.addrToNum())
            {
                if (routes[i].metric <= newRoute.metric)
                    betterRouteExists = true;
                else
                    routes.remove(i);
                break;
            }
        }
        if (!betterRouteExists)
        {
            addRoute(newRoute);
            updated = true;
        }
    }
    return updated;
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
        ripStr.append(",");
        for(int as:route.asIDs)
        {
            ripStr.append(QString::number(as));
            ripStr.append("-");
        }
        ripStr.append("#");
    }
    ripStr.append(gateway.ipAddr.toStr());
    return ripStr;
}

QString RoutingTable::toStringOSPF(IPv4 gateway)
{
    QString ripStr;

    for (Route route : routes)
    {
        ripStr.append(route.dest.ipAddr.toStr());
        ripStr.append(",");
        ripStr.append(route.dest.mask.toStr());
        ripStr.append(",");
        ripStr.append(QString::number(route.metric));
        ripStr.append(",");
        for(int as:route.asIDs)
        {
            ripStr.append(QString::number(as));
            ripStr.append("-");
        }
        ripStr.append("#");
    }
    ripStr.append(gateway.ipAddr.toStr());
    return ripStr;
}

Route::Route(IPv4 dest, const Mask &mask, IPv4 gateway, Port *port, int _metric, RouteGate _protocol) :
    dest(dest),
    mask(mask),
    gateway(gateway),
    port(port)
{
    metric = _metric;
    protocol = _protocol;
}
