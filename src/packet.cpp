#include "./headers/packet.h"
#include "./headers/ip.h"

#include "./headers/common.h"

QVector<QString> Packet::getPath() const
{
    return path;
}

void Packet::addToPath(QString newRouterAdd)
{
    // QMutexLocker lock(&mutex);
    myMutex.lock();
    path.append(newRouterAdd);
    myMutex.unlock();
}

int Packet::getQueueWaitCycles() const
{
    return queueWaitCycles;
}

void Packet::incQueueWaitCycles()
{
    queueWaitCycles++;
}

int Packet::getWaitCycles() const
{
    return waitCycles;
}

void Packet::incWaitCycles()
{
    waitCycles++;
}

PacketType Packet::getType() const
{
    return type;
}

int Packet::getPortID() const
{
    return portID;
}

void Packet::setPortID(int newPortID)
{
    portID = newPortID;
}

int Packet::getSeqNum() const
{
    return seqNum;
}

void Packet::setSeqNum(int newSeqNum)
{
    seqNum = newSeqNum;
}

int Packet::getFileSeqNum() const
{
    return fileSeqNum;
}

void Packet::setFileSeqNum(int newFileSeqNum)
{
    fileSeqNum = newFileSeqNum;
}

QByteArray Packet::getPayload() const
{
    return payload;
}

void Packet::setPayload(const QByteArray &newPayload)
{
    payload = newPayload;
}

Packet::Packet(QString _string, PacketType _type, IPVersion _ipVer, IPv4 _source, IPv4 _dest)
{
    type = _type;
    string = _string;
    ipVer = _ipVer;
    source = _source;
    dest = _dest;
    waitCycles = 0;
    queueWaitCycles = 0;
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
