#include "./headers/networkSimulator.h"
#include "./ui_networksimulator.h"
#include <QtConcurrent/QtConcurrent>

NetworkSimulator::NetworkSimulator(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::NetworkSimulator)
{
    ui->setupUi(this);
    tickDuration = DEFAULT_CYCLE_DURATION;
    protocol = OSPF;
    lambda = DEFAULT_LAMBDA;
    tickCount = DEFAULT_CYCLE_COUNT;

    networkThread = new QThread();
}

void NetworkSimulator::start()
{
    network = new Network(new EventHandler(tickDuration, tickCount), protocol, lambda);

    QObject::connect(network, &Network::oneCycleFinished,
                     this, &NetworkSimulator::tick);

    network->moveToThread(networkThread);
    networkThread->start();

    network->start();
}

NetworkSimulator::~NetworkSimulator()
{
    delete ui;
}

void NetworkSimulator::on_start_clicked()
{
    start();
}

void NetworkSimulator::on_cycleDuration_sliderMoved(int position)
{
    tickDuration = position;
    ui->tickDurationShow->setText(QString::number(position));
}

void NetworkSimulator::on_protocol_activated(int index)
{
    if (index == 0)
        protocol = OSPF;
    else if (index == 1)
        protocol = RIP;
}

void NetworkSimulator::on_lambda_valueChanged(int arg1)
{
    lambda = arg1;
}

void NetworkSimulator::on_tickCount_sliderMoved(int position)
{
    tickCount = position;
    ui->tickCountShow->setText(QString::number(position));
}

void NetworkSimulator::tick(int tickNum)
{
    if (tickNum >= tickCount)
    {
        disconnect(network, &Network::oneCycleFinished, this, 0);
        network->stop();
        network->printRoutingTables();
        qDebug() << "analysis:";
        qDebug() <<"total received packets:"<< network->getPacketsReceived();
        qDebug() <<"total sent packets:"<< network->getPacketsSent();
        qDebug() <<"total dropped packets:"<< network->getPacketsDropped();
        qDebug() <<"average queue waiting time:"<< network->getTotalQueueWaitCycles() / network->getPacketsReceived();
        qDebug() <<"average total waiting time:"<< network->getTotalWaitCycles() / network->getPacketsReceived();
        qDebug() <<"least waiting time:"<< network->getLeastWait();
        qDebug() <<"highest waiting time:"<< network->getHighestWait();
        qDebug() <<"highest queue waiting time:"<< network->getHighestQueueWait();
        qDebug() <<"least queue waiting time:"<< network->getLeastQueueWait();
        // delete eventHandler;
        delete network;
        // exit(0);
    }
}

