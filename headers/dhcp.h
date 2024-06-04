#ifndef DHCP_H
#define DHCP_H

#include <QThread>

#include "packet.h"
#include "port.h"
#include "ip.h"
#include "common.h"


class DhcpServer : public QObject
{
    Q_OBJECT
private:
    struct DhcpTableRow
    {
        IPv4 ip;
        int id; // MAC
        int lastLeaseTime;
        DhcpTableRow(const IPv4 &ip, int id, int lastLeaseTime);
    };
    IP* ip;
    IP* ipStart;
    IP* ipEnd;
    int clk;

    void removeIPs();
    bool ipAvailable(IPv4 ip);
    bool alreadyAssigned(QSharedPointer<Packet> packet);

    void assignIP(QSharedPointer<Packet> packet);
    void sendOffer(DhcpTableRow row);
public:
    explicit DhcpServer(IP* _ip, IP* _ipStart,IP* _ipEnd, Port* _port,  QObject *parent = nullptr);
    Port* port;
    QVector<QSharedPointer<Packet>> buffer;

    QVector<DhcpTableRow> dhcpTable;

    void recievePacket(QSharedPointer<Packet> packet);
    void sendPacket(QSharedPointer<Packet> packet);
    void tick(int _time);
};

#endif // DHCP_H
