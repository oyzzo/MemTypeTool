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
    this->ui->userEdit->setText(QString::fromStdString(this->cred->user).replace("\t","\\t").replace("\n","\\n").replace("\x16","⌚"));
    this->ui->hopEdit->setText(QString::fromStdString(this->cred->hop).replace("\t","\\t").replace("\n","\\n").replace("\x16","⌚"));
    this->ui->passwdEdit->setText(QString::fromStdString(this->cred->password).replace("\t","\\t").replace("\n","\\n").replace("\x16","⌚"));
    this->ui->submitEdit->setText(QString::fromStdString(this->cred->submit).replace("\t","\\t").replace("\n","\\n").replace("\x16","⌚"));
}

void CredentialEditWindow::saveCredential()
{
    this->cred->name = ui->nameEdit->text().toStdString();
    this->cred->user = ui->userEdit->text().replace("\\t","\t").replace("\\n","\n").replace("⌚","\x16").toStdString();
    this->cred->hop = ui->hopEdit->text().replace("\\t","\t").replace("\\n","\n").replace("⌚","\x16").toStdString();
    this->cred->password = ui->passwdEdit->text().replace("\\t","\t").replace("\\n","\n").replace("⌚","\x16").toStdString();
    this->cred->submit = ui->submitEdit->text().replace("\\t","\t").replace("\\n","\n").replace("⌚","\x16").toStdString();

    emit accepted();
    this->close();
}
void CredentialEditWindow::addDelay()
{
    this->ui->hopEdit->setText(this->ui->hopEdit->text().prepend("⌚")); /* Unicode watch char */
}
