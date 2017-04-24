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
    connect(ui->actionRead, &QAction::triggered, this, &MainWindow::readFromDevice);

    //Create a timmer for polling the state and info of the memtype
    QTimer * connectionTimer = new QTimer(this);
    connect(connectionTimer, SIGNAL(timeout()), this, SLOT(updateConnection()));
    connectionTimer->start(1000);
    this->stWidget = new statuswidget();
    ui->mainToolBar->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
    ui->mainToolBar->addWidget(stWidget);
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

void MainWindow::readFromDevice()
{
    if (!this->dev.present) {
        qDebug() << "No device found.";
        qDebug() << "Can't read from locked device.";
        QMessageBox msgBox(QMessageBox::Warning, tr("Warning"), "No device found.", 0, this);
            msgBox.setDetailedText("To read from your MemType device it has to be connected to a USB port. If the device is connected check the documentation to ensure the installation is Ok and your user has access rights to the USB.");
            msgBox.addButton(tr("&Ok"), QMessageBox::AcceptRole);
        msgBox.exec();
        return;
    }

    if (memtypeLocked()) {
        qDebug() << "Can't read from locked device.";
        QMessageBox msgBox(QMessageBox::Warning, tr("Warning"), "Device Locked.", 0, this);
            msgBox.setDetailedText("You must unlock your MemType device first by entering the P.I.N. using the joystick.");
            msgBox.addButton(tr("&Ok"), QMessageBox::AcceptRole);
        msgBox.exec();
        return;
    }

    if (this->mCredentials.length() != 0) {
       QMessageBox msgBox (QMessageBox::Information, tr("Info"), "Credentials from device will be added after current credentials, continue?",0,this);
       msgBox.addButton(tr("&Ok"), QMessageBox::AcceptRole);
       msgBox.addButton(tr("&Cancel"), QMessageBox::RejectRole);

       if (msgBox.exec() == QMessageBox::RejectRole)
           return;
    }

    Memtype_connect();

    uint8_t *cred_buff = (uint8_t*)malloc(MEMTYPE_BUFFER_SIZE);
    Memtype_read(cred_buff, MEMTYPE_BUFFER_SIZE, 0);
    int clen = Memtype_credLen(cred_buff, MEMTYPE_BUFFER_SIZE);
    memtype_credential_t *list = (memtype_credential_t *) malloc(sizeof(memtype_credential_t) * clen);

    Memtype_decrypt(list, clen, cred_buff, MEMTYPE_BUFFER_SIZE, 0);

    for (int i = 0; i < clen; i++) {
        auto cred = new Credential();
        cred->name = list[i].name;
        cred->user = list[i].user;
        cred->hop = list[i].hop;
        cred->password = list[i].pass;
        cred->submit = list[i].submit;

        this->mCredentials.append(cred);
    }
    free(list);
    free(cred_buff);

    Memtype_disconnect();

    renderCredentials();

    QMessageBox msgBox (QMessageBox::Information, tr("Info"), "Credential read complete.",0,this);
    msgBox.addButton(tr("&Ok"), QMessageBox::AcceptRole);
    msgBox.exec();

}

bool MainWindow::memtypeLocked()
{
    memtype_locked_t lock;
    lock = LOCKED;

    if (this->dev.present) {

        Memtype_connect();

        if (Memtype_isLocked(&lock) != NO_ERROR) {
            qDebug() << "Error checking lock on device.";
        }

        Memtype_disconnect();
    }

    return lock == LOCKED;
}

bool MainWindow::updateConnection()
{
    memtype_ret_t err;

    err = Memtype_connect();

    if (err == NO_ERROR) {
        //device plugged to usb!
        this->dev.present = true;
        this->stWidget->setConnected();
    } else {
        //No device plugged to usb
        this->dev.present = false;
        this->stWidget->setDisconnected();
    }

    Memtype_disconnect();

}
