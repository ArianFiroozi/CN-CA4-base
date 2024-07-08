#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include "ui_dialog.h"

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = nullptr);
    ~Dialog();
    friend class NetworkSimulator;

private slots:
    void on_OK_clicked();

protected:
    Ui::Dialog *ui;
};

#endif // DIALOG_H
