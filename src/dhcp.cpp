#include "./headers/dhcp.h"

using namespace std;

DhcpServer::DhcpServer(IP* _ip, IP* _ipStart, IP* _ipEnd, Port* _port, QObject *parent)
    : QObject{parent}
{
    ip = _ip;
    port = _port;
    ipStart = _ipStart;
    ipEnd = _ipEnd;
}

void DhcpServer::tick(int _time)
{
    clk = _time;
    removeIPs();
}

bool DhcpServer::ipAvailable(IPv4 ip)
{
    for (DhcpTableRow row:dhcpTable)
        if (row.ip.ipAddr.addrToNum() == ip.ipAddr.addrToNum())
            return false;
    return true;
}

bool DhcpServer::alreadyAssigned(QSharedPointer<Packet> packet)
{
    for (const DhcpTableRow &row:dhcpTable)
        if (row.id == packet->getString().toInt())
            return false;
    return true;
}

void DhcpServer::assignIP(QSharedPointer<Packet> packet)
{
    if (alreadyAssigned(packet))
        return;

    IPv4 ip(ipStart->mask, ipStart->ipAddr);
    for (int i=0;i<ipEnd->ipAddr.addrToNum() - ipStart->ipAddr.addrToNum();i++)
    {
        ip.ipAddr.hostID++;
        if (ipAvailable(ip))
            break;

        if (i == ipEnd->ipAddr.addrToNum() - ipStart->ipAddr.addrToNum() -1)
        {
            qDebug() << "DHCP server out of ip addresses!";
            return;
        }
    }
    dhcpTable.append(DhcpTableRow(ip, packet->getString().toInt(), clk));
}

void DhcpServer::sendOffer(DhcpTableRow row)
{
    sendPacket(QSharedPointer<Packet> (
        new Packet(row.ip.ipAddr.toStr(), DHCP_OFFER, IPV4, *(IPv4*)ip,
                   IPv4("255.255.255.255", "255.255.255.255"))));
}

void DhcpServer::recievePacket(QSharedPointer<Packet> packet)
{
    if (packet->getType() == DHCP_LEASE)
    {
        int deviceID = packet->getString().toInt();
        for (int i=0;i < dhcpTable.size();i++)
        {
            if (dhcpTable[i].id == deviceID)
                dhcpTable[i].lastLeaseTime = clk;
        }
    }
    else if (packet->getType() == DHCP_REQUEST)
    {
        qDebug() <<"DhcpServer recieved request from: "<<packet->getString();
        assignIP(packet);
        for(auto row:dhcpTable)
            if (row.id == packet->getString().toInt())
                sendOffer(row);
    }

}

void DhcpServer::sendPacket(QSharedPointer<Packet> packet)
{
    port->write(packet);
}

DhcpServer::DhcpTableRow::DhcpTableRow(const IPv4 &ip, int id, int lastLeaseTime) : ip(ip),
    id(id),
    lastLeaseTime(lastLeaseTime)
{}

void DhcpServer::removeIPs()
{
    for (int i = dhcpTable.size() - 1; i >=0 ;i++)
        if (dhcpTable[i].lastLeaseTime + LEASE_TIMEOUT)
            dhcpTable.remove(i);
}
