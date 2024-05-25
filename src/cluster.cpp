#include "../headers/cluster.h"

Router* Cluster::getRouter(int id)
{
    return id <= routers.size() ? routers[id-1] : NULL;
}

void Cluster::connectForward(EventHandler* eventHandler)
{
    for (Router* router : routers)
    {
        QObject::connect(eventHandler, &EventHandler::forwardSig,
                         router, &Router::forward);
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

Mesh::Mesh(int _x, int _y, IPv4 netAddrIP) // in this mesh, unlike final version each up router is connected to one pc
{
    makeDummyApp();

    x = _x;
    y = _y;

    for(int i=0;i<y;i++)
    {
        for (int j=0;j<x;j++)
        {
            routers.append(new Router(i*x+j+1));

            IPv4 newIP(netAddrIP.mask, netAddrIP.ipAddr);
            newIP.ipAddr.hostID += i*x + j + 1;
            routers.last()->ip = new IPv4(netAddrIP.mask, netAddrIP.ipAddr);
            connectRouters(i, j);
        }
    }
    //TODO: get tables according to static/dynamic type
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
    //only down can send to up TODO:make it both ways for dynamic
    //connecting network
    if (j==0)
    {
        routers.last()->addPort(new Port(2));
        if (i != y-1)
            routers.last()->addPort(new Port(3));

        if (i != 0)
            QObject::connect(routers[(i-1)*x+j]->getPortWithID(3), &Port::getPacket,
                             routers.last(), &Router::recievePacket);
    }
    else if (j==x-1)
    {
        if (i != y-1)
            routers.last()->addPort(new Port(3));

        QObject::connect(routers[i*x+j-1]->getPortWithID(2), &Port::getPacket,
                         routers.last(), &Router::recievePacket);
        if (i != 0)
            QObject::connect(routers[(i-1)*x+j]->getPortWithID(3), &Port::getPacket,
                             routers.last(), &Router::recievePacket);
    }
    else
    {
        routers.last()->addPort(new Port(2));
        if (i != y-1)
            routers.last()->addPort(new Port(3));

        QObject::connect(routers[i*x+j-1]->getPortWithID(2),
                         &Port::getPacket, routers.last(), &Router::recievePacket);
        if (i != 0)
            QObject::connect(routers[(i-1)*x+j]->getPortWithID(3),
                             &Port::getPacket, routers.last(), &Router::recievePacket);
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

    //this is a test. later this should be done with PC entity
    QVector<QString> upPcIPs;
    upPcIPs.append("192.168.20.1");
    upPcIPs.append("192.168.20.2");
    upPcIPs.append("192.168.20.3");
    upPcIPs.append("192.168.20.4");

    for(int i=0;i<ringLen;i++)
    {
        routers.append(new Router(i+1));

        IPv4 newIP(netAddrIP.mask, netAddrIP.ipAddr);
        newIP.ipAddr.hostID += i + 1;
        routers.last()->ip = new IPv4(netAddrIP.mask, netAddrIP.ipAddr);
        connectRingRouters(i);
    }

    routers.append(new Router(ringLen + 1)); //star
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
                         &Port::getPacket, routers.last(), &Router::recievePacket);
        QObject::connect(routers.last()->getPortWithID(4),
                         &Port::getPacket, routers[i-1], &Router::recievePacket);
    }
    if (i==ringLen-1){
        QObject::connect(routers.last()->getPortWithID(2),
                         &Port::getPacket, routers.first(), &Router::recievePacket);
        QObject::connect(routers.first()->getPortWithID(4),
                         &Port::getPacket, routers.last(), &Router::recievePacket);
    }
}

void RingStar::connectStarRouter()
{
    for (int i=0;i<starConnections.length();i++)
    {
        routers.last()->addPort(new Port(starConnections[i]));

        QObject::connect(routers.last()->getPortWithID(starConnections[i]),
                         &Port::getPacket, routers[starConnections[i]], &Router::recievePacket);
        QObject::connect(routers[starConnections[i]]->getPortWithID(3),
                         &Port::getPacket, routers.last(), &Router::recievePacket);
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
