#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H

#include <QObject>
#include <QThread>

class EventHandler : public QThread
{
    Q_OBJECT

private:
    int period;
    bool stopFlag;
    double stopTime;

public:
    EventHandler(int microSeconds, double _stopTime=1000000);

    void changePeriod(int microSeconds);
    void start();
    void stop();
signals:
    void tick(double time);
    void startSig();
    void stopSig();
};

#endif // EVENTHANDLER_H
