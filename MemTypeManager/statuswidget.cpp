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
