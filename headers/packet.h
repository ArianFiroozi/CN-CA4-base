#ifndef PACKET_H
#define PACKET_H

#include <QString>
#include <QVector>

#include "ip.h"

enum IPVersion{IPV4, IPV6};

class Packet
{
private:
    QString string;
    IPv4 source, dest;
    QVector<QString> path;
    int queueWaitCycles;
    int waitCycles;

public:
    Packet(QString _string="", IPVersion _ipVer=IPV4, IPv4 _source=IPv4(), IPv4 _dest=IPv4());

    IPVersion ipVer;
    void setString(const QString &newString);
    QString getString() const;
    IPv4 getSource() const;
    IPv4 getDest() const;

    QString toStr();
    QVector<QString> getPath() const;
    void addToPath(QString newRouterAdd);
    int getQueueWaitCycles() const;
    void incQueueWaitCycles();
    int getWaitCycles() const;
    void incWaitCycles();
};

#endif // PACKET_H
