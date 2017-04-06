#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "credentialwidget.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Initialize attributes
    this->credWindow = NULL;

    //The quit menu action
    connect(ui->actionQuit, &QAction::triggered, QApplication::instance(), &QApplication::quit);

    //The add credential button
    connect(ui->addButton, &QPushButton::clicked, this, &MainWindow::addCredential);

}

MainWindow::~MainWindow()
{
    delete ui;

    //clean up the credentialsi
    for (auto t : this->mCredentials) {
        delete t;
    }

    if (this->credWindow) {
        delete credWindow;
    }
}

// This function adds a new credential to the mCredentials vector
void MainWindow::addCredential()
{
    auto cred = new Credential();
    cred->name = "New Credential";
    this->mCredentials.append(cred);
    this->ui->progressBar->setValue(this->ui->progressBar->value()+1);
    this->renderCredentials();
}

// This function deletes a credential from the mCredentials vector
void MainWindow::deleteCredential(Credential* credential)
{
    if (this->mCredentials.contains(credential)) {
        this->mCredentials.remove(this->mCredentials.indexOf(credential));
        this->ui->progressBar->setValue(this->ui->progressBar->value()-1);
        this->renderCredentials();
    }
}

// This function opens the credential editor window
void MainWindow::editCredential(Credential* credential)
{
    this->credWindow = new CredentialEditWindow();

    //Update screen to show up modifications (if there are any)
    connect(credWindow, &CredentialEditWindow::accepted, this, &MainWindow::renderCredentials);

    credWindow->setCredential(credential);
    credWindow->show();
}

void MainWindow::moveUpCredential(Credential* credential)
{
    auto pos = this->mCredentials.indexOf(credential);
    if (pos > 0) {
        auto aux = this->mCredentials.at(pos - 1);
        this->mCredentials.replace(pos - 1, credential);
        this->mCredentials.replace(pos, aux);
        this->renderCredentials();
    }

}

void MainWindow::moveDownCredential(Credential* credential)
{
    auto pos = this->mCredentials.indexOf(credential);
    if (pos < this->mCredentials.count() - 1) {
        auto aux = this->mCredentials.at(pos +1);
        this->mCredentials.replace(pos + 1, credential);
        this->mCredentials.replace(pos, aux);
        this->renderCredentials();
    }

}

void MainWindow::renderCredentials()
{
    while (!this->ui->scrollLayout->isEmpty()){
        QWidget* w = this->ui->scrollLayout->itemAt(0)->widget();
        w->setVisible(false);
        this->ui->scrollLayout->removeWidget(w);
    }

    for (auto t : this->mCredentials) {
        auto credWidget = new credentialWidget();
        connect(credWidget, &credentialWidget::deleteCredential, this, &MainWindow::deleteCredential);
        connect(credWidget, &credentialWidget::editCredential, this, &MainWindow::editCredential);
        connect(credWidget, &credentialWidget::upCredential, this, &MainWindow::moveUpCredential);
        connect(credWidget, &credentialWidget::downCredential, this, &MainWindow::moveDownCredential);

        credWidget->setCredentialName(QString::fromStdString(t->name));
        credWidget->cred = t;
        this->ui->scrollLayout->addWidget(credWidget);
    }
}
