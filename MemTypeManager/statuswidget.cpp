#include "statuswidget.h"
#include "ui_statuswidget.h"

statuswidget::statuswidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::statuswidget)
{
    ui->setupUi(this);
}

statuswidget::~statuswidget()
{
    delete ui;
}

void statuswidget::setConnected()
{
    ui->statusButton->setEnabled(true);
    ui->statusButton->setToolTip("Device Connected");
    ui->statusButton->setStatusTip("Device connected.");
}

void statuswidget::setDisconnected()
{
    ui->statusButton->setEnabled(false);
    ui->statusButton->setToolTip("No device found");
    ui->statusButton->setStatusTip("No device found.");
}
