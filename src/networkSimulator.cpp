#include "./headers/networkSimulator.h"
#include "./ui_networksimulator.h"

NetworkSimulator::NetworkSimulator(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::NetworkSimulator)
{
    ui->setupUi(this);
    tickDuration = DEFAULT_CYCLE_DURATION;
    protocol = OSPF;
    lambda = DEFAULT_LAMBDA;
    tickCount = DEFAULT_CYCLE_COUNT;
}

void NetworkSimulator::start()
{
    // ui->start->setEnabled(false);

    eventHandler = new EventHandler(tickDuration, tickCount);
    network = new Network(eventHandler, protocol, lambda);

    QObject::connect(network, &Network::oneCycleFinished, this, &NetworkSimulator::tick);
    QObject::connect(eventHandler, &EventHandler::tick, network, &Network::tick);

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
        network->stop();
        network->printRoutingTables();
        qDebug() << network->getPacketsReceived();
        qDebug() << network->getPacketsSent();
        qDebug() << network->getPacketsDropped();
        qDebug() << network->getTotalQueueWaitCycles() / network->getPacketsReceived();
        qDebug() << network->getTotalWaitCycles() / network->getPacketsReceived();


        exit(0);
    }
}

