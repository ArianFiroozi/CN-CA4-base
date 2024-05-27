#include "../headers/cluster.h"

Router* Cluster::getRouter(int id)
{
    return id <= routers.size() ? routers[id-1] : NULL;
}

void Cluster::connectTick(EventHandler* eventHandler)
{
    for (Router* router : routers)
    {
        QObject::connect(eventHandler, &EventHandler::tick,
                         router, &Router::tick);
    }
}

void Cluster::printRoutingTables()
{
    for (Router* router:routers)
    {
        cout << endl << "routing table " << router->id << ":" << endl;
        router->routingTable.print();
    }
}

Mesh::Mesh(int _x, int _y, IPv4 netAddrIP, RoutingProtocol _protocol) // in this mesh, unlike final version each up router is connected to one pc
{
    makeDummyApp();

    x = _x;
    y = _y;
    protocol = _protocol;

    for(int i=0;i<y;i++)
    {
        for (int j=0;j<x;j++)
        {
            IPv4 newIP(netAddrIP.mask, netAddrIP.ipAddr);
            newIP.ipAddr.hostID += i*x + j + 1;
            routers.append(new Router(i*x+j+1, new IPv4(newIP.mask.toStr(), newIP.ipAddr.toStr()), _protocol));
            connectRouters(i, j);
        }
    }

    if (protocol == MANUAL)
        getStaticRoutingTables();

    for (Router* router:routers)
    {
        threads.append(new QThread());
        router->moveToThread(threads.last());
        threads.last()->start();
    }
}

Mesh::~Mesh()
{
    for (auto thread : threads)
    {
        thread->exit();
        thread->deleteLater();
    }
}

void Cluster::makeDummyApp()
{
    int dummy_argc = 0;
    char x = 'b';
    char *dummy_argv[1] = {&x};
    dummy = new QCoreApplication(dummy_argc, dummy_argv);
}

void Mesh::connectRouters(int i, int j)
{
    if (j==0) // first column
    {
        routers.last()->addPort(new Port(2));
        if (i != y-1)
            routers.last()->addPort(new Port(3));
        if (i != 0)
            routers.last()->addPort(new Port(1));

        if (i != 0)
        {
            QObject::connect(routers[(i-1)*x+j]->getPortWithID(3), &Port::getPacket,
                             routers.last(), &Router::recievePacket, Qt::ConnectionType::QueuedConnection);
            QObject::connect(routers.last()->getPortWithID(1), &Port::getPacket,
                             routers[(i-1)*x+j], &Router::recievePacket, Qt::ConnectionType::QueuedConnection);
        }
    }
    else if (j==x-1) // last cloumn
    {
        routers.last()->addPort(new Port(4));

        if (i != y-1)
            routers.last()->addPort(new Port(3));
        if (i != 0)
            routers.last()->addPort(new Port(1));

        QObject::connect(routers[i*x+j-1]->getPortWithID(2), &Port::getPacket,
                         routers.last(), &Router::recievePacket, Qt::ConnectionType::QueuedConnection);
        QObject::connect(routers.last()->getPortWithID(4), &Port::getPacket,
                         routers[i*x+j-1], &Router::recievePacket, Qt::ConnectionType::QueuedConnection);

        if (i != 0)
        {
            QObject::connect(routers[(i-1)*x+j]->getPortWithID(3), &Port::getPacket,
                             routers.last(), &Router::recievePacket, Qt::ConnectionType::QueuedConnection);
            QObject::connect(routers.last()->getPortWithID(1), &Port::getPacket,
                             routers[(i-1)*x+j], &Router::recievePacket, Qt::ConnectionType::QueuedConnection);
        }
    }
    else
    {
        routers.last()->addPort(new Port(2));
        routers.last()->addPort(new Port(4));
        if (i != y-1)
            routers.last()->addPort(new Port(3));
        if (i != 0)
            routers.last()->addPort(new Port(1));

        QObject::connect(routers[i*x+j-1]->getPortWithID(2),
                         &Port::getPacket, routers.last(), &Router::recievePacket, Qt::ConnectionType::QueuedConnection);
        QObject::connect(routers.last()->getPortWithID(4), &Port::getPacket,
                         routers[i*x+j-1], &Router::recievePacket, Qt::ConnectionType::QueuedConnection);
        if (i != 0)
        {
            QObject::connect(routers[(i-1)*x+j]->getPortWithID(3),
                             &Port::getPacket, routers.last(), &Router::recievePacket, Qt::ConnectionType::QueuedConnection);
            QObject::connect(routers.last()->getPortWithID(1), &Port::getPacket,
                             routers[(i-1)*x+j], &Router::recievePacket, Qt::ConnectionType::QueuedConnection);
        }
    }
}

void Mesh::getStaticRoutingTables()
{
    for (Router* router:routers) //static
    {
        QString path("../resources/routingTables/manualMesh4x4/routingTable");
        path.append(QString::number(router->id));
        path.append(".csv");

        for (Port* port : router->ports)
            router->routingTable.initFromFile(path, port);
    }
}

RingStar::RingStar(int _ringLen, QVector<int> _starConnections, IPv4 netAddrIP)
{
    makeDummyApp();

    ringLen = _ringLen;
    starConnections = _starConnections;

    for(int i=0;i<ringLen;i++)
    {

        IPv4 newIP(netAddrIP.mask, netAddrIP.ipAddr);
        newIP.ipAddr.hostID += i + 1;
        routers.append(new Router(i+1, new IPv4(netAddrIP.mask, netAddrIP.ipAddr)));
        connectRingRouters(i);
    }

    IPv4 newIP(netAddrIP.mask, netAddrIP.ipAddr);
    newIP.ipAddr.hostID += ringLen + 1;
    routers.append(new Router(ringLen + 1, new IPv4(netAddrIP.mask, netAddrIP.ipAddr))); //star
    connectStarRouter();

    //TODO: get tables according to static/dynamic type
    getStaticRoutingTables();

    for (Router* router:routers)
    {
        threads.append(new QThread());
        router->moveToThread(threads.last());
        threads.last()->start();
    }
}

void RingStar::connectRingRouters(int i)
{
    routers.last()->addPort(new Port(2));
    routers.last()->addPort(new Port(3)); //for star
    routers.last()->addPort(new Port(4));

    if (i!=0)
    {
        QObject::connect(routers[i-1]->getPortWithID(2),
                         &Port::getPacket, routers.last(), &Router::recievePacket, Qt::ConnectionType::QueuedConnection);
        QObject::connect(routers.last()->getPortWithID(4),
                         &Port::getPacket, routers[i-1], &Router::recievePacket, Qt::ConnectionType::QueuedConnection);
    }
    if (i==ringLen-1){
        QObject::connect(routers.last()->getPortWithID(2),
                         &Port::getPacket, routers.first(), &Router::recievePacket, Qt::ConnectionType::QueuedConnection);
        QObject::connect(routers.first()->getPortWithID(4),
                         &Port::getPacket, routers.last(), &Router::recievePacket, Qt::ConnectionType::QueuedConnection);
    }
}

void RingStar::connectStarRouter()
{
    for (int i=0;i<starConnections.length();i++)
    {
        routers.last()->addPort(new Port(starConnections[i]));

        QObject::connect(routers.last()->getPortWithID(starConnections[i]),
                         &Port::getPacket, routers[starConnections[i]-1], &Router::recievePacket, Qt::ConnectionType::QueuedConnection);
        QObject::connect(routers[starConnections[i]-1]->getPortWithID(3),
                         &Port::getPacket, routers.last(), &Router::recievePacket, Qt::ConnectionType::QueuedConnection);
    }
}

void RingStar::getStaticRoutingTables()
{
    for (Router* router:routers) //static
    {
        QString path("../resources/routingTables/manualRingStar/routingTable");
        path.append(QString::number(router->id));
        path.append(".csv");

        for (Port* port : router->ports)
            router->routingTable.initFromFile(path, port);
    }
}
