#ifndef NETWORKSIMULATOR_H
#define NETWORKSIMULATOR_H

#include <QMainWindow>

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
    ~NetworkSimulator();

private:
    Ui::NetworkSimulator *ui;
};
#endif // NETWORKSIMULATOR_H
