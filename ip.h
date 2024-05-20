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
        num += netID1 * pow(2, 8);
        num += netID2 * pow(2, 16);
        num += netID3 * pow(2, 24);
        return num;
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
    virtual bool includes(IP* other)=0;

    IPAddr ipAddr;
protected:
    Mask mask;
};

class IPv4:IP
{
    int maskToInt();
    IPAddr netAddr();
    IPAddr broadcastAddr();

public:
    bool includes(IP* other);
    IPv4(QString maskStr, QString ipAddrStr);
    QString getIPStr();
};

// class IPv6 //TODO: add if i have time
// {
//     int ip;
// };


#endif // IP_H
