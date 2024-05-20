#include "ip.h"

int IPv4::maskToInt()
{
    int maskInt = 0;
    maskInt += log2(256 - mask.hostID);
    maskInt += log2(256 - mask.netID3);
    maskInt += log2(256 - mask.netID2);
    maskInt += log2(256 - mask.netID1);
    return 32 - maskInt;
}

IPv4::IPv4(QString maskStr, QString ipAddrStr)
{
    ipAddr.strToIP(ipAddrStr);
    mask.strToMask(maskStr);
}

QString IPv4::getIPStr()
{
    QString IPStr = "";
    IPStr.append(QString::number(ipAddr.netID1));
    IPStr.append(".");
    IPStr.append(QString::number(ipAddr.netID2));
    IPStr.append(".");
    IPStr.append(QString::number(ipAddr.netID3));
    IPStr.append(".");
    IPStr.append(QString::number(ipAddr.hostID));
    IPStr.append("/");
    IPStr.append(QString::number(maskToInt()));
    return IPStr;
}

IPAddr IPv4::broadcastAddr()
{
    IPAddr broadcastAddress;
    broadcastAddress.netID1 = this->ipAddr.netID1 | (~this->ipAddr.netID1 & 0xFF);
    broadcastAddress.netID2 = this->ipAddr.netID2 | (~this->ipAddr.netID2 & 0xFF);
    broadcastAddress.netID3 = this->ipAddr.netID3 | (~this->ipAddr.netID3 & 0xFF);
    broadcastAddress.hostID = this->ipAddr.hostID | (~this->ipAddr.hostID & 0xFF);
    return broadcastAddress;
}

IPAddr IPv4::netAddr()
{
    IPAddr networkAddress;
    networkAddress.netID1 = this->ipAddr.netID1 & this->ipAddr.netID1;
    networkAddress.netID2 = this->ipAddr.netID2 & this->ipAddr.netID2;
    networkAddress.netID3 = this->ipAddr.netID3 & this->ipAddr.netID3;
    networkAddress.hostID = this->ipAddr.hostID & this->ipAddr.hostID;
    return networkAddress;
}

bool IPv4::includes(IP* other)
{
    uint32_t otherAddr = other->ipAddr.addrToNum();
    return (otherAddr < this->broadcastAddr().addrToNum() && otherAddr > this->netAddr().addrToNum());
}
