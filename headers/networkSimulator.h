#ifndef NETWORKSIMULATOR_H
#define NETWORKSIMULATOR_H

#include <QMainWindow>
#include <QVector>
#include "network.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class NetworkSimulator;
}
QT_END_NAMESPACE

class NetworkSimulator : public QMainWindow
{
    Q_OBJECT

public:
    NetworkSimulator(QWidget *parent = nullptr);
    void start();
    ~NetworkSimulator();

private slots:
    void on_start_clicked();
    void on_cycleDuration_sliderMoved(int position);
    void on_protocol_activated(int index);
    void on_lambda_valueChanged(int arg1);
    void on_tickCount_sliderMoved(int position);

private:
    Network* network;
    EventHandler* eventHandler;
    // QThread* networkThread;

    int tickDuration;
    int lambda;
    int tickCount;
    QVector<QThread*> threads;

    RoutingProtocol protocol;

    Ui::NetworkSimulator *ui;

    void tick(int tickNum);
};

#endif // NETWORKSIMULATOR_H
