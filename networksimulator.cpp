#include "networksimulator.h"
#include "ui_networksimulator.h"

NetworkSimulator::NetworkSimulator(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::NetworkSimulator)
{
    ui->setupUi(this);
}

NetworkSimulator::~NetworkSimulator()
{
    delete ui;
}
