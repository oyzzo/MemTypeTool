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

    //The clean credentials button
    connect(ui->cleanButton, &QPushButton::clicked, this, &MainWindow::cleanCredentialList);

    //The Load Keyboard Layout button
    connect(ui->actionSet_KeyLayout, &QAction::triggered, this, &MainWindow::loadLayout);

    //The export creedentials button
    connect(ui->actionExport_File, &QAction::triggered, this, &MainWindow::exportCredentials);

    //The import credentials button
    connect(ui->actionImport_File, &QAction::triggered, this, &MainWindow::importCredentials);

    //The read credentials button
    connect(ui->actionRead, &QAction::triggered, this, &MainWindow::readFromDevice);

    //The show credentials button
    connect(ui->seeButton, &QPushButton::pressed, this, &MainWindow::showPin);

    //The hide credentials button
    connect(ui->seeButton, &QPushButton::released, this, &MainWindow::hidePin);

    //Validate the PIN
    connect(ui->pinEdit, &QLineEdit::editingFinished, this, &MainWindow::validatePIN);

    //Create a timmer for polling the state and info of the memtype
    this->connectionTimer = new QTimer(this);
    connect(connectionTimer, SIGNAL(timeout()), this, SLOT(updateConnection()));
    connectionTimer->start(1000);
    this->stWidget = new statuswidget();
    ui->mainToolBar->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
    ui->mainToolBar->addWidget(stWidget);
    ui->pinEdit->setValidator(new QIntValidator);
}

MainWindow::~MainWindow()
{
    delete ui;

    //clean up the credentials
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
    uint16_t size = 0;

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
    this->connectionTimer->stop();
    if (!this->dev.present) {
        qDebug() << "No device found.";
        qDebug() << "Can't read from locked device.";
        QMessageBox msgBox(QMessageBox::Warning, tr("Warning"), "No device found.", 0, this);
            msgBox.setDetailedText("To read from your MemType device it has to be connected to a USB port. If the device is connected check the documentation to ensure the installation is Ok and your user has access rights to the USB.");
            msgBox.addButton(tr("&Ok"), QMessageBox::AcceptRole);
        msgBox.exec();
        this->connectionTimer->start(1000);
        return;
    }

    if (memtypeLocked()) {
        qDebug() << "Can't read from locked device.";
        QMessageBox msgBox(QMessageBox::Warning, tr("Warning"), "Device Locked.", 0, this);
            msgBox.setDetailedText("You must unlock your MemType device first by entering the P.I.N. using the joystick.");
            msgBox.addButton(tr("&Ok"), QMessageBox::AcceptRole);
        msgBox.exec();
        this->connectionTimer->start(1000);
        return;
    }

    if (this->mCredentials.length() != 0) {
       QMessageBox msgBox (QMessageBox::Information, tr("Info"), "Credentials from device will be added after current credentials, continue?",0,this);
       msgBox.addButton(tr("&Ok"), QMessageBox::AcceptRole);
       msgBox.addButton(tr("&Cancel"), QMessageBox::RejectRole);

       if (msgBox.exec() == QMessageBox::RejectRole) {
           this->connectionTimer->start(1000);
           return;
       }
    }

    if (ui->pinEdit->text().length() != 4) {
       QMessageBox msgBox (QMessageBox::Information, tr("Info"), "Please introduce the device PIN to decrypt credentials.",0,this);
       msgBox.addButton(tr("&Ok"), QMessageBox::AcceptRole);

       msgBox.exec();
       this->connectionTimer->start(1000);
       return;
    }


    Memtype_connect();

    uint8_t *cred_buff = (uint8_t*)malloc(MEMTYPE_BUFFER_SIZE);
    Memtype_readProgress(cred_buff, MEMTYPE_BUFFER_SIZE, 0);
    int clen = Memtype_credLen(cred_buff, MEMTYPE_BUFFER_SIZE);
    memtype_credential_t *list = (memtype_credential_t *) malloc(sizeof(memtype_credential_t) * clen);



    Memtype_decrypt(list, clen, cred_buff, MEMTYPE_BUFFER_SIZE, ui->pinEdit->text().toInt());

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
    this->connectionTimer->start(1000);
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
    return this->dev.present;
}

memtype_ret_t MainWindow::Memtype_readProgress(uint8_t * block, uint16_t len, uint16_t offset)
{
    QProgressDialog progress("Reading...", "&Cancel", 0, len, this);
    progress.show();


    memtype_ret_t ret;
    uint8_t msg[8] = { 2, 0, 0, 0, 0, 0, 0, 0 };
    uint8_t l = sizeof(msg);
    uint16_t i;
    uint8_t *buff = block;

    /* Prepare command for offset and size */
    msg[1] = offset & 0xFF;
    msg[2] = (offset >> 8) & 0xFF;
    msg[3] = len & 0xFF;
    msg[4] = (len >> 8) & 0xFF;

    if ((block != NULL) && ((len + offset) <= 2048) && ((len % 8) == 0)) {
        /* Send CMD */
        QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
        ret = memtype_send(msg, &l);
        QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);


        if (msg[0] == 2) {
            /* Read Data */
            for (i = 0; i < len; i += 8) {
                progress.setValue(i);
                QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
                l = 8;
                memtype_receive(&buff[i], &l);
                QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);

            }
        } else {
            ret = ERROR;
        }

    } else {
        ret = ERROR;
    }
    progress.setValue(len);
    QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    return ret;
}

void MainWindow::validatePIN()
{
    if (ui->pinEdit->text().length() != 4) {
        QMessageBox msgBox (QMessageBox::Information, tr("Info"), "PIN must be a 4 digit number from 0000 to 9999.",0,this);
        msgBox.addButton(tr("&Ok"), QMessageBox::AcceptRole);
        msgBox.exec();
        ui->pinEdit->setText("");
    }
}

void MainWindow::showPin()
{
    ui->pinEdit->setEchoMode(QLineEdit::Normal);
}

void MainWindow::hidePin()
{
    ui->pinEdit->setEchoMode(QLineEdit::Password);
}

void MainWindow::cleanCredentialList()
{

    if (this->mCredentials.length() != 0) {
       QMessageBox msgBox (QMessageBox::Information, tr("Info"), "All the credentials will be deleted from the list, continue?",0,this);
       msgBox.addButton(tr("&Ok"), QMessageBox::AcceptRole);
       msgBox.addButton(tr("&Cancel"), QMessageBox::RejectRole);

       if (msgBox.exec() == QMessageBox::RejectRole) {
           return;
       }
    }

     //clean up the credentials
    this->mCredentials.clear();
    renderCredentials();
}
