#ifndef ROUTER_H
#define ROUTER_H

#include <QObject>
#include <QThread>
#include <QMutex>
#include <QVector>

#include "packet.h"
#include "link.h"

struct IP
{
};
struct IPv4:IP
{
    int ip;
};
struct IPv6
{
    int ip;
};  // may be class

class Router : public QThread
{
public:
    explicit Router(int _id, QThread *parent = nullptr);
    IP* ip;
    int id;
    QVector<Packet*> inBuff;
    QVector<Packet*> outBuff;
    QVector<Link*> links;

    QMutex* mutex;

    void recievePacket(Packet* packet, int recieveID);
    bool sendPacket(Packet* packet, int sendID);

    void start();
    void stop();
    void queuePacket(Packet* packet);
    void addLink(Link* link, int routerID);
signals:
};

#endif // ROUTER_H
