#ifndef CLUSTER_H
#define CLUSTER_H

#include "router.h"

#include <QVector>
#include <QCoreApplication>
#include "eventHandler.h"

class Cluster
{
protected:
    void makeDummyApp();
    QCoreApplication* dummy;

public:
    QVector<Router*> routers;
    Router* getRouter(int id);
    QVector<QThread*> threads;

    void connectForward(EventHandler* eventHandler);
    void printRoutingTables();
};

class Mesh : public Cluster
{
private:
    int x, y;

    void connectRouters(int i, int j);
    void getStaticRoutingTables();

public:
    Mesh(int _x, int _y, IPv4 netAddIP);
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
    RingStar(int _ringLen, QVector<int> _starConnections, IPv4 netAddrIP);
};

#endif // CLUSTER_H
