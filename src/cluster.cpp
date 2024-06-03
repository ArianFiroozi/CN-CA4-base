#include "../headers/cluster.h"
#include "../headers/common.h"

#include <QFile>

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
        qDebug() << "\nrouting table " << router->id << ":";
        router->routingTable.print();
    }
}

void Cluster::addPortDelays(QString address)
{
    for (Router* router:routers)
    {
        QFile file(address);

        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            cout << address.toStdString()<<" no such file!"<<endl;
            exit(0);
            return;
        }

        QTextStream in(&file);
        while (!in.atEnd())
        {
            QString line = in.readLine();

            if(line.startsWith("//")) continue;

            QVector<QString> lineVec = line.split(",");
            int routerID  = lineVec[0].toInt();
            if (routerID != router->id) continue;

            router->getPortWithID(lineVec[1].toInt())->delay = lineVec[2].toInt();
        }
    }
}

Mesh::Mesh(int _x, int _y, IPv4 netAddrIP, RoutingProtocol _protocol, bool delayedPorts)
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

    if (delayedPorts)
        addPortDelays(MESH_DELAY_PATH);

    for (Router* router:routers)
    {
        threads.append(new QThread());
        router->moveToThread(threads.last());
        threads.last()->start();
    }
}

Mesh::~Mesh()
{
    dummy->exit();
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
        QString path(MESH_ROUTING_TABLE_PATH);
        path.append(QString::number(router->id));
        path.append(".csv");

        for (Port* port : router->ports)
            router->routingTable.initFromFile(path, port);
    }
}

RingStar::RingStar(int _ringLen, QVector<int> _starConnections, IPv4 netAddrIP, RoutingProtocol _protocol, bool delayedPorts)
{
    makeDummyApp();

    ringLen = _ringLen;
    starConnections = _starConnections;
    protocol = _protocol;

    for(int i=0;i<ringLen;i++)
    {

        IPv4 newIP(netAddrIP.mask, netAddrIP.ipAddr);
        newIP.ipAddr.hostID += i + 1;
        routers.append(new Router(i+1, new IPv4(newIP.mask.toStr(), newIP.ipAddr.toStr()), protocol));
        connectRingRouters(i);
    }

    IPv4 newIP(netAddrIP.mask, netAddrIP.ipAddr);
    newIP.ipAddr.hostID += ringLen + 1;
    routers.append(new Router(ringLen + 1, new IPv4(newIP.mask.toStr(), newIP.ipAddr.toStr()), protocol)); //star
    connectStarRouter();

    if (protocol == MANUAL)
        getStaticRoutingTables();

    if (delayedPorts)
        addPortDelays(RING_STAR_DELAY_PATH);

    for (Router* router:routers)
    {
        threads.append(new QThread());
        router->moveToThread(threads.last());
        threads.last()->start();
    }
}

RingStar::~RingStar()
{
    dummy->exit();
    for (auto thread : threads)
    {
        thread->exit();
        thread->deleteLater();
    }
}

void RingStar::connectRingRouters(int i)
{
    routers.last()->addPort(new Port(2));
    routers.last()->addPort(new Port(10 + i + 1)); //for star
    routers.last()->addPort(new Port(3));
    routers.last()->addPort(new Port(4));

    if (i!=0)
    {
        QObject::connect(routers[i-1]->getPortWithID(2), &Port::getPacket,
                         routers.last(), &Router::recievePacket, Qt::ConnectionType::QueuedConnection);
        QObject::connect(routers.last()->getPortWithID(4), &Port::getPacket,
                         routers[i-1], &Router::recievePacket, Qt::ConnectionType::QueuedConnection);
    }
    if (i==ringLen-1){
        QObject::connect(routers.last()->getPortWithID(2), &Port::getPacket,
                         routers.first(), &Router::recievePacket, Qt::ConnectionType::QueuedConnection);
        QObject::connect(routers.first()->getPortWithID(4), &Port::getPacket,
                         routers.last(), &Router::recievePacket, Qt::ConnectionType::QueuedConnection);
    }
}

void RingStar::connectStarRouter()
{
    for (int i=0;i<starConnections.length();i++)
    {
        routers.last()->addPort(new Port(10 + starConnections[i]));

        QObject::connect(routers.last()->getPortWithID(10 + starConnections[i]),
                         &Port::getPacket, routers[starConnections[i]-1], &Router::recievePacket, Qt::ConnectionType::QueuedConnection);
        QObject::connect(routers[starConnections[i]-1]->getPortWithID(10 + starConnections[i]),
                         &Port::getPacket, routers.last(), &Router::recievePacket, Qt::ConnectionType::QueuedConnection);
    }
}

void RingStar::getStaticRoutingTables()
{
    for (Router* router:routers) //static
    {
        QString path(RING_STAR_ROUTING_TABLE_PATH);
        path.append(QString::number(router->id));
        path.append(".csv");

        for (Port* port : router->ports)
            router->routingTable.initFromFile(path, port);
    }
}

void Torus::connectRouters(int i, int j)
{
    if (j==0) //first column
    {
        routers[i*x+j]->addPort(new Port(4));
        if (i == y-1)
            routers[i*x+j]->addPort(new Port(3));
        if (i == 0)
            routers[i*x+j]->addPort(new Port(1));

        if (i == y-1)
        {
            QObject::connect(routers[0]->getPortWithID(1), &Port::getPacket,
                             routers[i*x+j], &Router::recievePacket, Qt::ConnectionType::QueuedConnection);
            QObject::connect(routers[i*x+j]->getPortWithID(3), &Port::getPacket,
                             routers[0], &Router::recievePacket, Qt::ConnectionType::QueuedConnection);
        }
    }
    else if (j==x-1) //last cloumn
    {
        routers[i*x+j]->addPort(new Port(2));

        if (i == y-1)
            routers[i*x+j]->addPort(new Port(3));
        if (i == 0)
            routers[i*x+j]->addPort(new Port(1));

        QObject::connect(routers[i*x]->getPortWithID(4), &Port::getPacket,
                         routers[i*x+j], &Router::recievePacket, Qt::ConnectionType::QueuedConnection);
        QObject::connect(routers[i*x+j]->getPortWithID(2), &Port::getPacket,
                         routers[i*x], &Router::recievePacket, Qt::ConnectionType::QueuedConnection);

        if (i == y-1)
        {
            QObject::connect(routers[x-1]->getPortWithID(1), &Port::getPacket,
                             routers[i*x+j], &Router::recievePacket, Qt::ConnectionType::QueuedConnection);
            QObject::connect(routers[i*x+j]->getPortWithID(3), &Port::getPacket,
                             routers[x-1], &Router::recievePacket, Qt::ConnectionType::QueuedConnection);
        }
    }
    else
    {
        if (i == y-1)
            routers[i*x+j]->addPort(new Port(3));
        if (i == 0)
            routers[i*x+j]->addPort(new Port(1));

        if (i == y-1)
        {
            QObject::connect(routers[j]->getPortWithID(1), &Port::getPacket,
                             routers[i*x+j], &Router::recievePacket, Qt::ConnectionType::QueuedConnection);
            QObject::connect(routers[i*x+j]->getPortWithID(3), &Port::getPacket,
                             routers[j], &Router::recievePacket, Qt::ConnectionType::QueuedConnection);
        }
    }
}

Torus::Torus(int _x, int _y, const IPv4 &netAddIP, RoutingProtocol _protocol, bool delayedPorts)
    : Mesh(_x, _y, netAddIP, _protocol, false)
{
    for(int i=0;i<y;i++)
        for (int j=0;j<x;j++)
            connectRouters(i, j);

    if (delayedPorts)
        addPortDelays(TORUS_DELAY_PATH);
}
