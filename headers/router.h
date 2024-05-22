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
    QVector<Packet*> buffer;
    // QVector<Port*> ports;
    RoutingTable routingTable;

    QMutex* mutex;

    void recievePacket(Packet* packet, int recieveID);
    bool sendPacket(Packet* packet, int sendID);

    void start();
    void stop();
    void queuePacket(Packet* packet);
    void addLink(Port* link, int routerID);
signals:
};

#endif // ROUTER_H
