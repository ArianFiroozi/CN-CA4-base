#include "./headers/networkSimulator.h"
#include "./ui_networksimulator.h"
#include "./ui_dialog.h"
#include <QtConcurrent/QtConcurrent>

NetworkSimulator::NetworkSimulator(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::NetworkSimulator)
    , dialogUi(new Dialog())
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

QString NetworkSimulator::analyseStr()
{
    QString output = "analysis:\n";
    output += "total received packets: " + QString::number(network->getPacketsReceived()) + "\n";
    output += "total sent packets: " + QString::number(network->getPacketsSent()) + "\n";
    output += "total dropped packets: " + QString::number(network->getPacketsDropped()) + "\n";
    output += "average queue waiting time: " + QString::number(network->getTotalQueueWaitCycles() / network->getPacketsReceived()) + "\n";
    output += "average total waiting time: " + QString::number(network->getTotalWaitCycles() / network->getPacketsReceived()) + "\n";
    output += "least waiting time: " + QString::number(network->getLeastWait()) + "\n";
    output += "highest waiting time: " + QString::number(network->getHighestWait()) + "\n";
    output += "highest queue waiting time: " + QString::number(network->getHighestQueueWait()) + "\n";
    output += "least queue waiting time: " + QString::number(network->getLeastQueueWait()) + "\n";

    return output;
}

void NetworkSimulator::tick(int tickNum)
{
    if (tickNum >= tickCount)
    {
        this->ui->progressBar->setValue(100);

        disconnect(network, &Network::oneCycleFinished, this, 0);
        network->stop();
        network->printRoutingTables();
        QString output = analyseStr();
        qDebug().noquote()<<output.toStdString();

        delete network;
        networkThread->quit();
        dialogUi->ui->textBrowser->setText(output);
        dialogUi->show();
    }
    else
    {
        this->ui->progressBar->setValue((float)tickNum/float(tickCount)*100);
    }
}

