#include "./headers/ip.h"

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
    QString IPStr = ipAddr.toStr();
    IPStr.append("/");
    IPStr.append(QString::number(maskToInt()));
    return IPStr;
}

IPAddr IPv4::broadcastAddr()
{
    IPAddr broadcastAddress;
    broadcastAddress.netID1 = this->ipAddr.netID1 | (~this->mask.netID1 & 0xFF);
    broadcastAddress.netID2 = this->ipAddr.netID2 | (~this->mask.netID2 & 0xFF);
    broadcastAddress.netID3 = this->ipAddr.netID3 | (~this->mask.netID3 & 0xFF);
    broadcastAddress.hostID = this->ipAddr.hostID | (~this->mask.hostID & 0xFF);
    return broadcastAddress;
}

IPAddr IPv4::netAddr()
{
    IPAddr networkAddress;
    networkAddress.netID1 = this->ipAddr.netID1 & this->mask.netID1;
    networkAddress.netID2 = this->ipAddr.netID2 & this->mask.netID2;
    networkAddress.netID3 = this->ipAddr.netID3 & this->mask.netID3;
    networkAddress.hostID = this->ipAddr.hostID & this->mask.hostID;
    return networkAddress;
}

bool IPv4::includes(IPv4 other)
{
    uint32_t otherAddr = other.ipAddr.addrToNum();
    return (otherAddr <= this->broadcastAddr().addrToNum() && otherAddr >= this->netAddr().addrToNum());
}

IPv4::IPv4(Mask _mask, IPAddr _ipAddr)
{
    mask = _mask;
    ipAddr = _ipAddr;
}

bool IPv6::includes(IPv6 other)
{
    return mapToIPv4().includes(other.mapToIPv4());
}

IPv6::IPv6(int prefix, QString ipAddrStr)
{
    ipAddr.strToIP(ipAddrStr);
    prefix = prefix;
}

IPv6::IPv6(int prefix, IP6Addr _ipAddr)
{
    ipAddr = _ipAddr;
    prefix = prefix;
}

QString IPv6::getIPStr()
{
    QString IPStrNaive = ipAddr.toStr();
    QString IPStr;

    bool isShort=false;
    bool shortening = false;
    for (QString portion:IPStrNaive.split(":"))
    {
        if (portion == "0000")
        {
            if(!isShort)
            {
                shortening = true;
                continue;
            }
            else
            {
                IPStr.append("0:");
                continue;
            }
        }
        else
        {
            if (shortening==true)
            {
                isShort=true;
                shortening=false;
                IPStr.append(":");
            }

            bool removedBackZeros = false;
            for (auto c:portion)
            {
                if (c!='0')
                {
                    removedBackZeros=true;
                    IPStr.append(c);
                }
                else if (removedBackZeros)
                    IPStr.append(c);

            }
            IPStr.append(':');
        }
    }
    IPStr.removeLast();

    IPStr.append("/");
    IPStr.append(QString::number(prefix));
    return IPStr;
}

IPv4 IPv6::mapToIPv4()
{
    IPv4 other;
    other.ipAddr.netID1 = ipAddr.interfaceID1%256;
    other.ipAddr.netID2 = ipAddr.interfaceID2%256;
    other.ipAddr.netID3 = ipAddr.interfaceID3%256;
    other.ipAddr.hostID = ipAddr.interfaceID4%256;

    int maskNum = prefix - 96;
    other.mask.hostID = maskNum & 0xFF;
    other.mask.netID3 = (maskNum >> 8) & 0xFF;
    other.mask.netID2 = (maskNum >> 16) & 0xFF;
    other.mask.netID1 = (maskNum >> 24) & 0xFF;
    return other;
}

IPv6 IPv6::mapFromIPv4(IPv4 other)
{
    IPv6 new6(128);
    new6.ipAddr.netID1=0xFFFF;
    new6.ipAddr.netID2=0;
    new6.ipAddr.netID3=0;
    new6.ipAddr.netID4=0;
    new6.ipAddr.interfaceID1 = other.ipAddr.netID1;
    new6.ipAddr.interfaceID2 = other.ipAddr.netID2;
    new6.ipAddr.interfaceID3 = other.ipAddr.netID3;
    new6.ipAddr.interfaceID4 = other.ipAddr.hostID;
    new6.prefix = other.maskToInt() + 96;
    return new6;
}
