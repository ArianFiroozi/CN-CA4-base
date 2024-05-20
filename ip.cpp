#include "ip.h"

IP::IP() {}

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
