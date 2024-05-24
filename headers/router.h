#ifndef ROUTER_H
#define ROUTER_H

#include <QObject>
#include <QThread>
#include <QMutex>
#include <QVector>

#include "packet.h"
#include "port.h"
#include "ip.h"
#include "routingtable.h"

class Router : public QThread
{
public:
    explicit Router(int _id, QThread *parent = nullptr);
    IP* ip;
    int id;
    QVector<Port*> ports;
    QVector<Packet> buffer;
    RoutingTable routingTable;

    QMutex* mutex;

    void recievePacket(Packet packet);
    bool sendPacket(Packet packet);
    void forward();

    void start();
    void stop();
    void queuePacket(Packet packet);
    void addPort(Port* port);

    Port* getPortWithID(int portID);
signals:
    void packetSent(Port* newPort);
};

#endif // ROUTER_H
