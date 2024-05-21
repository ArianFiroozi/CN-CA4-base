#include "./headers/packet.h"
#include "./headers/ip.h"

Packet::Packet(QString _string, IPVersion _ipVer, IPv4 _source, IPv4 _dest)
{
    string = _string;
    ipVer = _ipVer;
    source = _source;
    dest = _dest;
}

void Packet::setString(const QString &newString)
{
    string = newString;
}

IPv4 Packet::getSource() const
{
    return source;
}

IPv4 Packet::getDest() const
{
    return dest;
}

QString Packet::toStr()
{
    QString packetStr;
    packetStr.append((ipVer == IPV4)?"IPv4":"IPv6");
    packetStr.append(" ");
    packetStr.append(source.getIPStr());
    packetStr.append(" ");
    packetStr.append(dest.getIPStr());
    packetStr.append(" ");
    packetStr.append(string);
    return packetStr;
}

QString Packet::getString() const
{
    return string;
}
