#ifndef PACKET_H
#define PACKET_H

#include <QString>
#include <QVector>
#include <QMutex>

#include "ip.h"

enum IPVersion{IPV4, IPV6};

enum PacketType{HELLO, LSA, ROUTING_TABLE_RIP, DHCP_OFFER, DHCP_REQUEST, DHCP_LEASE, MSG};

class Packet
{
private:
    QMutex mutex;
    PacketType type;
    QString string;
    IPv4 source, dest;
    QVector<QString> path;
    int queueWaitCycles;
    int waitCycles;
    int portID;

public:
    Packet(QString _string="", PacketType _type=MSG, IPVersion _ipVer=IPV4, IPv4 _source=IPv4(), IPv4 _dest=IPv4());

    IPVersion ipVer;
    void setString(const QString &newString);
    QString getString() const;
    IPv4 getSource() const;
    IPv4 getDest() const;

    QString toStr();
    QVector<QString> getPath() const;
    void addToPath(QString newRouterAdd);
    int getQueueWaitCycles() const;
    void incQueueWaitCycles();
    int getWaitCycles() const;
    void incWaitCycles();
    PacketType getType() const;
    int getPortID() const;
    void setPortID(int newPortID);
};

#endif // PACKET_H
