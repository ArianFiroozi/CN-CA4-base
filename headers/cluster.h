#ifndef CLUSTER_H
#define CLUSTER_H

#include "router.h"

#include <QVector>
#include <QCoreApplication>
#include "eventHandler.h"
#include "dhcp.h"

class Cluster
{
protected:
    void makeDummyApp();
    QCoreApplication* dummy;
    RoutingProtocol protocol;
    DhcpServer* dhcpServer;
    void addPortDelays(QString address);

public:
    Cluster(IPv4* dhcpIP=new IPv4("255.255.255.255", "0.0.0.0"), IPv4* dhcpStart=new IPv4("255.255.255.255", "192.168.20.0"),
            IPv4* dhcpEnd=new IPv4("255.255.255.255", "192.168.20.250"));
    Cluster(DhcpServer*);
    QVector<Router*> routers;
    Router* getRouter(int id);
    QVector<QThread*> threads;

    void connectTick(EventHandler* eventHandler);
    void printRoutingTables();
};

class Mesh : public Cluster
{
protected:
    int x, y;

    void connectRouters(int i, int j);
    void getStaticRoutingTables();

public:
    Mesh(int _x, int _y, IPv4 netAddIP, RoutingProtocol _protocol=MANUAL, bool delayedPorts=false, DhcpServer* _dhcpServer=NULL);
    ~Mesh();
};

class RingStar:public Cluster
{
private:
    int ringLen;
    QVector<int> starConnections;

    void connectRingRouters(int i);
    void connectStarRouter();
    void getStaticRoutingTables();

public:
    RingStar(int _ringLen, QVector<int> _starConnections, IPv4 netAddrIP, RoutingProtocol _protocol=MANUAL, bool delayedPorts=false, DhcpServer* _dhcpServer=NULL);
    ~RingStar();
};

class Torus : public Mesh
{
    void connectRouters(int i, int j);
public:
    Torus(int _x, int _y, const IPv4 &netAddIP, RoutingProtocol _protocol = RIP, bool delayedPorts=false, DhcpServer* _dhcpServer=NULL);

};

#endif // CLUSTER_H
