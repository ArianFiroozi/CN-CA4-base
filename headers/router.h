#ifndef ROUTER_H
#define ROUTER_H

#include <QObject>
#include <QThread>
#include <QMutex>
#include <QVector>
#include <QSharedPointer>

#include "packet.h"
#include "port.h"
#include "ip.h"
#include "routingtable.h"

enum RoutingProtocol
{
    RIP,
    BGP,
    OSPF,
    MANUAL
};

class Router : public QThread
{
public:
    explicit Router(int _id, RoutingProtocol _protocol = MANUAL, QThread *parent = nullptr);
    IPv4* ip;
    int id;
    QVector<Port*> ports;
    QVector<QSharedPointer<Packet>> buffer;
    RoutingTable routingTable;
    RoutingProtocol protocol;

    QMutex* mutex;

    void recievePacket(QSharedPointer<Packet> packet);
    bool sendPacket(QSharedPointer<Packet> packet);
    void forward();
    void tick();

    void start();
    void stop();
    void queuePacket(Packet packet);
    void addPort(Port* port);

    Port* getPortWithID(int portID);
signals:
    void packetSent(Port* newPort);
};

#endif // ROUTER_H
