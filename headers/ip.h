#ifndef IP_H
#define IP_H

#include <QString>
#include <QStringList>
#include <QKeySequence>

#include <math.h>
#include <iostream>
#include <sstream>
#include <iomanip>

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

struct Addr6
{
    int netID1;
    int netID2;
    int netID3;
    int netID4;
    int interfaceID1;
    int interfaceID2;
    int interfaceID3;
    int interfaceID4;

    quint128 addrToNum()
    {
        uint32_t num = 0;
        num += interfaceID4;
        num += interfaceID3 * pow(2, 16);
        num += interfaceID2 * pow(2, 32);
        num += interfaceID1 * pow(2, 48);
        num += netID4 * pow(2, 64);
        num += netID3 * pow(2, 80);
        num += netID2 * pow(2, 96);
        num += netID1 * pow(2, 112);
        return num;
    }

    QString toStr()
    {
        std::stringstream ss;
        ss << std::hex << std::setfill('0');
        ss << std::setw(4) << netID1 << ":" << std::setw(4) << netID2 << ":" << std::setw(4)
           << netID3 << ":" << std::setw(4) << netID4 << ":";
        ss << std::setw(4) << interfaceID1 << ":" << std::setw(4) << interfaceID2 << ":" << std::setw(4)
           << interfaceID3 << ":" << std::setw(4) << interfaceID4;
        std::string sstd = ss.str();
        return QString(QString::fromStdString(sstd));
    }
};

struct IP6Addr:Addr6
{
    void strToIP(QString ipv6String) {
        std::vector<int> values;

        std::stringstream ss(ipv6String.toStdString());
        std::string token;

        while (std::getline(ss, token, ':')) {
            values.push_back(std::stoi(token, 0, 16));
        }

        if (values.size() == 8) {
            netID1 = values[0];
            netID2 = values[1];
            netID3 = values[2];
            netID4 = values[3];
            interfaceID1 = values[4];
            interfaceID2 = values[5];
            interfaceID3 = values[6];
            interfaceID4 = values[7];
        }
    }
};

class IPv6
{
public:
    IP6Addr ipAddr;
    int prefix;

    IPv6(int prefix, QString ipAddrStr="0000:0000:0000:0000:0000:0000:0000:0000");
    IPv6(int prefix, IP6Addr _ipAddr);
    QString getIPStr();
    IPv4 mapToIPv4();
    static IPv6 mapFromIPv4(IPv4 other);
    bool includes(IPv6 other);
};

#endif // IP_H
