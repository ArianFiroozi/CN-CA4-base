#ifndef IP_H
#define IP_H

#include <QString>
#include <QStringList>

#include <math.h>

class IP
{
public:
    IP();
protected:
    struct
    {
        int netID1;
        int netID2;
        int netID3;
        int hostID;

        void strToIP(QString ipAddrStr) {
            QStringList parts = ipAddrStr.split(".");
            netID1 = parts[0].toInt();
            netID2 = parts[1].toInt();
            netID3 = parts[2].toInt();
            hostID = parts[3].toInt();
        }
    }ipAddr;

    struct
    {
        int netID1;
        int netID2;
        int netID3;
        int hostID;
        void strToMask(QString maskStr) {
            QStringList parts = maskStr.split(".");
            netID1 = parts[0].toInt();
            netID2 = parts[1].toInt();
            netID3 = parts[2].toInt();
            hostID = parts[3].toInt();
        }
    }mask;
};

class IPv4:IP
{
    int maskToInt();

public:
    IPv4(QString maskStr, QString ipAddrStr);
    QString getIPStr();
};

class IPv6 //TODO: add if i have time
{
    int ip;
};


#endif // IP_H
