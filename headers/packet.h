#ifndef PACKET_H
#define PACKET_H

#include <QString>
#include "ip.h"

enum IPVersion{IPV4, IPV6};

class Packet
{
private:
    QString string;
    IPv4 source, dest;

public:
    Packet(QString _string="", IPVersion _ipVer=IPV4, IPv4 _source=IPv4(), IPv4 _dest=IPv4());

    IPVersion ipVer;
    void setString(const QString &newString);
    QString getString() const;
    IPv4 getSource() const;
    IPv4 getDest() const;

    QString toStr();
};

#endif // PACKET_H
