#ifndef IP_H
#define IP_H

#include <QString>
#include <QStringList>

#include <math.h>

struct Addr
{
    int netID1;
    int netID2;
    int netID3;
    int hostID;

    uint32_t addrToNum()
    {
        uint32_t num = 0;
        num += hostID;
        num += netID3 * pow(2, 8);
        num += netID2 * pow(2, 16);
        num += netID1 * pow(2, 24);
        return num;
    }

    QString toStr()
    {
        QString addrStr = "";
        addrStr.append(QString::number(netID1));
        addrStr.append(".");
        addrStr.append(QString::number(netID2));
        addrStr.append(".");
        addrStr.append(QString::number(netID3));
        addrStr.append(".");
        addrStr.append(QString::number(hostID));
        return addrStr;
    }
};

struct IPAddr:Addr
{
    void strToIP(QString ipAddrStr) {
        QStringList parts = ipAddrStr.split(".");
        netID1 = parts[0].toInt();
        netID2 = parts[1].toInt();
        netID3 = parts[2].toInt();
        hostID = parts[3].toInt();
    }
};

struct Mask:Addr
{
    void strToMask(QString maskStr) {
        QStringList parts = maskStr.split(".");
        netID1 = parts[0].toInt();
        netID2 = parts[1].toInt();
        netID3 = parts[2].toInt();
        hostID = parts[3].toInt();
    }
};

class IP
{
public:
    virtual int maskToInt()=0;
    virtual IPAddr netAddr()=0;
    virtual IPAddr broadcastAddr()=0;
    // virtual bool includes(IPv4 other)=0;
    virtual QString getIPStr()=0;

    IPAddr ipAddr;
    Mask mask;
};

class IPv4:public IP
{
public:
    int maskToInt();
    IPAddr netAddr();
    IPAddr broadcastAddr();

    bool includes(IPv4 other);
    IPv4(QString maskStr="0.0.0.0", QString ipAddrStr="0.0.0.0");
    IPv4(Mask _mask, IPAddr _ipAddr); // duplicate
    QString getIPStr();
};

// class IPv6 //TODO: add if i have time
// {
//     int ip;
// };


#endif // IP_H
