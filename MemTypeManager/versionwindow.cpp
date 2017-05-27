#include "versionwindow.h"
#include "ui_versionwindow.h"

VersionWindow::VersionWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VersionWindow)
{
    ui->setupUi(this);
    connect(ui->closeButton, &QPushButton::clicked, this, &VersionWindow::close);
}

VersionWindow::~VersionWindow()
{
    delete ui;
}
