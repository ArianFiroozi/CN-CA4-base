#ifndef ROUTER_H
#define ROUTER_H

#include <QObject>
#include <QThread>

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
    explicit Router(QThread *parent = nullptr);
    IP* ip;

    void getPacket();
    bool sendPacket();
signals:
};

#endif // ROUTER_H
