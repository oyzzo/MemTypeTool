#include "mainwindow.h"
#include "ui_mainwindow.h"



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

    //The Load Keyboard Layout button
    connect(ui->actionSet_KeyLayout, &QAction::triggered, this, &MainWindow::loadLayout);

    //The export creedentials button
    connect(ui->actionExport_File, &QAction::triggered, this, &MainWindow::exportCredentials);

    //The import credentials button
    connect(ui->actionImport_File, &QAction::triggered, this, &MainWindow::importCredentials);

    //The read credentials button
    connect(ui->actionRead, &QAction::triggered, this, &MainWindow::memtypeLocked);

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

// Load Keyboard Layout
void MainWindow::loadLayout()
{
    QString filename = QFileDialog::getOpenFileName(this);

    if (!filename.isEmpty()) {
        qDebug() << filename;
    }
}

// Import credentials file
void MainWindow::importCredentials()
{
    QString filename = QFileDialog::getOpenFileName(this);

    if (!filename.isEmpty()) {
        qDebug() << filename;
    }
}

// Export Credentials file
void MainWindow::exportCredentials()
{
    QString filename = QFileDialog::getSaveFileName(this);

    if (!filename.isEmpty()) {
        qDebug() << filename;
    }
}

// This function adds a new credential to the mCredentials vector
void MainWindow::addCredential()
{
    auto cred = new Credential();
    cred->name = "New Credential";
    this->mCredentials.append(cred);
    this->renderCredentials();
}

// This function deletes a credential from the mCredentials vector
void MainWindow::deleteCredential(Credential* credential)
{
    if (this->mCredentials.contains(credential)) {
        this->mCredentials.remove(this->mCredentials.indexOf(credential));
        this->renderCredentials();
    }
}

// This function opens the credential editor window
void MainWindow::editCredential(Credential* credential)
{
    if (!this->credWindow) {
        this->credWindow = new CredentialEditWindow();
        //Update screen to show up modifications (if there are any)
        connect(credWindow, &CredentialEditWindow::accepted, this, &MainWindow::renderCredentials);
    }

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
    int size = 0;

    while (!this->ui->scrollLayout->isEmpty()){
        QWidget* w = this->ui->scrollLayout->itemAt(0)->widget();
        w->setVisible(false);
        this->ui->scrollLayout->removeWidget(w);
    }

    for (auto t : this->mCredentials) {
        auto credWidget = new credentialWidget();
        size += t->Size();
        connect(credWidget, &credentialWidget::deleteCredential, this, &MainWindow::deleteCredential);
        connect(credWidget, &credentialWidget::editCredential, this, &MainWindow::editCredential);
        connect(credWidget, &credentialWidget::upCredential, this, &MainWindow::moveUpCredential);
        connect(credWidget, &credentialWidget::downCredential, this, &MainWindow::moveDownCredential);

        credWidget->setCredentialName(QString::fromStdString(t->name));
        credWidget->cred = t;
        this->ui->scrollLayout->addWidget(credWidget);
    }

    this->ui->progressBar->setValue(size);
}

bool MainWindow::memtypeLocked()
{
    Memtype_connect();

    memtype_locked_t lock;

    if (Memtype_isLocked(&lock) == NO_ERROR) {
        if (lock == LOCKED) {
            qDebug() << "LOCKED!";
        } else {
            qDebug() << "UUUUUUUUNNNNN LOCKEEEEDDDD!!!";
        }
    }

    Memtype_disconnect();

    return lock == LOCKED;
}
