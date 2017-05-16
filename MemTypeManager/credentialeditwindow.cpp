#include "credentialeditwindow.h"
#include "ui_credentialeditwindow.h"

CredentialEditWindow::CredentialEditWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CredentialEditWindow)
{
    ui->setupUi(this);
    connect(ui->cancelCredentialButton, &QPushButton::clicked, this, &CredentialEditWindow::close);
    connect(ui->okCredentialButton, &QPushButton::clicked, this, &CredentialEditWindow::saveCredential);
    connect(ui->delayButton, &QPushButton::clicked, this, &CredentialEditWindow::addDelay);
}

CredentialEditWindow::~CredentialEditWindow()
{
    delete ui;
}

void CredentialEditWindow::setCredential(Credential* credential)
{
    this->cred = credential;
    this->ui->nameEdit->setText(QString::fromStdString(this->cred->name));
    this->ui->userEdit->setText(QString::fromStdString(this->cred->user));
    this->ui->hopEdit->setText(QString::fromStdString(this->cred->hop));
    this->ui->passwdEdit->setText(QString::fromStdString(this->cred->password));
    this->ui->submitEdit->setText(QString::fromStdString(this->cred->submit));
}

void CredentialEditWindow::saveCredential()
{
    this->cred->name = ui->nameEdit->text().toStdString();
    this->cred->user = ui->userEdit->text().toStdString();
    this->cred->hop = ui->hopEdit->text().toStdString();
    this->cred->password = ui->passwdEdit->text().toStdString();
    this->cred->submit = ui->submitEdit->text().toStdString();

    emit accepted();
    this->close();
}
void CredentialEditWindow::addDelay()
{
    this->ui->hopEdit->setText(this->ui->hopEdit->text().prepend("⌚")); /* Unicode watch char */
}
