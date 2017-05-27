#include "credentialwidget.h"
#include "ui_credentialwidget.h"
#include "mainwindow.h"

credentialWidget::credentialWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::credentialWidget)
{
    ui->setupUi(this);
    connect(ui->deleteButton, &QPushButton::clicked, this, &credentialWidget::emitDeleteCredential);
    connect(ui->credLabel, &QPushButton::clicked, this, &credentialWidget::emitEditCredential);
    connect(ui->upButton, &QPushButton::clicked, this, &credentialWidget::emitUpCredential);
    connect(ui->downButton, &QPushButton::clicked, this, &credentialWidget::emitDownCredential);
}

credentialWidget::~credentialWidget()
{
    delete ui;
}

void credentialWidget::emitDeleteCredential(void)
{
    emit deleteCredential(cred);
}

void credentialWidget::emitEditCredential(void)
{
    emit editCredential(cred);
}

void credentialWidget::emitUpCredential(void)
{
    emit upCredential(cred);
}

void credentialWidget::emitDownCredential(void)
{
    emit downCredential(cred);
}

void credentialWidget::setCredentialName(QString name)
{
    ui->credLabel->setText(name);
}

