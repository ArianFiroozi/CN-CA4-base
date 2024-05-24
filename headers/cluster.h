#ifndef CLUSTER_H
#define CLUSTER_H

#include "router.h"

#include <QVector>
#include <QCoreApplication>

class Cluster
{
public:
    QVector<Router*> routers;
    Router* getRouter(int id);

    void printRoutingTables();
};

class Mesh:public Cluster
{
private:
    void makeDummyApp();

    QCoreApplication* dummy;
    int x, y;
    QVector<QThread*> threads;
    void connectRouters(int i, int j);
    
    void getStaticRoutingTables();

public:
    Mesh(int _x, int _y, IPv4 netAddIP);
};

class RingStar:public Cluster
{
private:
public:
    RingStar();
};

#endif // CLUSTER_H
