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

    //The write credentials button
    connect(ui->actionWrite, &QAction::triggered, this, &MainWindow::writeToDevice);

    //The set pin credentials button
    connect(ui->actionSet_PIN, &QAction::triggered, this, &MainWindow::setPinToDevice);

    //The version window
    connect(ui->actionMemTypeTool, &QAction::triggered, this, &MainWindow::showVersion);

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
    uint8_t buffer[MEMTYPE_KEYBOARD_SIZE];
    int i = 0;
    QString filename = QFileDialog::getOpenFileName(this);

    this->connectionTimer->stop();

    if (!this->dev.present) {
        QMessageBox msgBox(QMessageBox::Warning, tr("Warning"), "No device found.", 0, this);
            msgBox.setDetailedText("The MemType device has to be connected to a USB port. If the device is connected check the documentation to ensure the installation is Ok and your user has access rights to the USB.");
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

    if (!filename.isEmpty()) {
        QFile file(filename);
        if (!file.open(QIODevice::ReadOnly)) {
            QMessageBox msgBox (QMessageBox::Warning, tr("Info"), file.errorString(),0,this);
            msgBox.addButton(tr("&Ok"), QMessageBox::AcceptRole);
            msgBox.exec();
            return;
        }

        if (!file.atEnd()) {
            QByteArray line = file.readLine();
            auto bytes = line.split(',');
            for (i = 0; i < bytes.length(); i++) {
                buffer[i] = bytes.at(i).toInt();
            }
        }
       Memtype_connect();
       if (Memtype_write_keyboard(buffer) == ERROR) {
           QMessageBox msgBox (QMessageBox::Warning, tr("Info"), "Error writting the keyboard layout to the device.",0,this);
           msgBox.addButton(tr("&Ok"), QMessageBox::AcceptRole);
           msgBox.exec();
       }
       Memtype_disconnect();
       QMessageBox msgBox (QMessageBox::Information, tr("Info"), "Set keyboard layout complete.",0,this);
       msgBox.addButton(tr("&Ok"), QMessageBox::AcceptRole);
       msgBox.exec();
       this->connectionTimer->start(1000);
    }
    cleanPIN();
}

// Import credentials file
void MainWindow::importCredentials()
{
    QString filename = QFileDialog::getOpenFileName(this);

    if (!filename.isEmpty()) {
        QFile file(filename);
        file.open(QIODevice::ReadOnly);
        QDataStream in(&file);

        // Read and check the header
        quint32 magic;
        in >> magic;
        if (magic != MAGIC_FILE_KIND) {
            QMessageBox msgBox (QMessageBox::Information, tr("Info"), "Wrong file kind.",0,this);
            msgBox.addButton(tr("&Ok"), QMessageBox::AcceptRole);
            msgBox.exec();
            file.close();
            return;
        }

        // Read the version
        qint32 version;
        in >> version;
        if (version < MEMTYPE_FORMAT_VERSION) {
            QMessageBox msgBox (QMessageBox::Information, tr("Info"), "File format too old.",0,this);
            msgBox.addButton(tr("&Ok"), QMessageBox::AcceptRole);
            msgBox.exec();
            file.close();
            return;
        }
        if (version > MEMTYPE_FORMAT_VERSION) {
            QMessageBox msgBox (QMessageBox::Information, tr("Info"), "File format too new.",0,this);
            msgBox.addButton(tr("&Ok"), QMessageBox::AcceptRole);
            msgBox.exec();
            file.close();
            return;
        }

        if (version == MEMTYPE_FORMAT_VERSION)
            in.setVersion(QDataStream::Qt_5_5);

        qint32 cred_length;
        in >> cred_length;

        QString aux;

        for (int i = 0; i < cred_length; i++) {
            auto cred = new Credential();
            in >> aux;
            cred->name = aux.toStdString();
            in >> aux;
            cred->user = aux.toStdString();
            in >> aux;
            cred->hop = aux.toStdString();
            in >> aux;
            cred->password = aux.toStdString();
            in >> aux;
            cred->submit = aux.toStdString();

            this->mCredentials.append(cred);
        }
        this->renderCredentials();

        QMessageBox msgBox (QMessageBox::Information, tr("Info"), "Credentials import complete.",0,this);
        msgBox.addButton(tr("&Ok"), QMessageBox::AcceptRole);
        msgBox.exec();

        file.close();
    }
}

// Export Credentials file
void MainWindow::exportCredentials()
{
    QString filename = QFileDialog::getSaveFileName(this);

    if (!filename.isEmpty()) {

        QFile file(filename);
        file.open(QIODevice::WriteOnly);
        QDataStream out(&file);

        // Write a header with a "magic number", a version and the credentials length.
        out << (quint32)MAGIC_FILE_KIND;
        out << (qint32)MEMTYPE_FORMAT_VERSION;
        out << (qint32)this->mCredentials.length();

        out.setVersion(QDataStream::Qt_5_5);

        // Write the data
        for (auto t : this->mCredentials) {
            out << QString().fromStdString(t->name);
            out << QString().fromStdString(t->user);
            out << QString().fromStdString(t->hop);
            out << QString().fromStdString(t->password);
            out << QString().fromStdString(t->submit);
        }

        QMessageBox msgBox (QMessageBox::Information, tr("Info"), "Credentials export complete.",0,this);
        msgBox.addButton(tr("&Ok"), QMessageBox::AcceptRole);
        msgBox.exec();

        file.close();
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
    this->credentialsSize = 0;

    while (!this->ui->scrollLayout->isEmpty()){
        QWidget* w = this->ui->scrollLayout->itemAt(0)->widget();
        w->setVisible(false);
        this->ui->scrollLayout->removeWidget(w);
    }

    for (auto t : this->mCredentials) {
        auto credWidget = new credentialWidget();
        this->credentialsSize += t->Size();
        connect(credWidget, &credentialWidget::deleteCredential, this, &MainWindow::deleteCredential);
        connect(credWidget, &credentialWidget::editCredential, this, &MainWindow::editCredential);
        connect(credWidget, &credentialWidget::upCredential, this, &MainWindow::moveUpCredential);
        connect(credWidget, &credentialWidget::downCredential, this, &MainWindow::moveDownCredential);

        credWidget->setCredentialName(QString::fromStdString(t->name));
        credWidget->cred = t;
        this->ui->scrollLayout->addWidget(credWidget);
    }

    if (this->credentialsSize < 2048) {
        this->ui->progressBar->setValue(this->credentialsSize);
        this->sizeMessage = false;
    } else {
        this->ui->progressBar->setValue(2048);
        if (!this->sizeMessage) {
            this->sizeMessage = true;
            QMessageBox msgBox (QMessageBox::Information, tr("Info"), "The credential list is bigger than the device memory.\nYou can work with any number of credentials but remember if you write them into a MemType, some of them may not fit into the memory.\n\nRemember to save the credential list as a file.",0,this);
            msgBox.addButton(tr("&Ok"), QMessageBox::AcceptRole);
            msgBox.exec();
        }
    }
}

void MainWindow::readFromDevice()
{
    this->connectionTimer->stop();
    if (!this->dev.present) {
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

    if (!checkPINToDevice(ui->pinEdit->text().toInt())) {
       QMessageBox msgBox (QMessageBox::Information, tr("Info"), "The PIN is incorrect!",0,this);
       msgBox.addButton(tr("&Ok"), QMessageBox::AcceptRole);
       msgBox.exec();
       this->connectionTimer->start(1000);
       cleanPIN();
       return;
    }

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
    cleanPIN();
}

void MainWindow::writeToDevice()
{
    this->connectionTimer->stop();
    if (!this->dev.present) {
        QMessageBox msgBox(QMessageBox::Warning, tr("Warning"), "No device found.", 0, this);
            msgBox.setDetailedText("To write to your MemType device it has to be connected to a USB port. If the device is connected check the documentation to ensure the installation is Ok and your user has access rights to the USB.");
            msgBox.addButton(tr("&Ok"), QMessageBox::AcceptRole);
        msgBox.exec();
        this->connectionTimer->start(1000);
        return;
    }

    if (memtypeLocked()) {
        QMessageBox msgBox(QMessageBox::Warning, tr("Warning"), "Device Locked.", 0, this);
            msgBox.setDetailedText("You must unlock your MemType device first by entering the P.I.N. using the joystick.");
            msgBox.addButton(tr("&Ok"), QMessageBox::AcceptRole);
        msgBox.exec();
        this->connectionTimer->start(1000);
        return;
    }

    if (this->credentialsSize > MEMTYPE_BUFFER_SIZE) {
       QMessageBox msgBox (QMessageBox::Information, tr("Info"), "Some credentials won't fit into the device memory. The credentials that won't fit will be marked.\nRemember to save files to prevent credential loss.\n\nContinue?",0,this);
       msgBox.addButton(tr("&Ok"), QMessageBox::AcceptRole);
       msgBox.addButton(tr("&Cancel"), QMessageBox::RejectRole);

       if (msgBox.exec() == QMessageBox::RejectRole) {
           this->connectionTimer->start(1000);
           return;
       }
    }

    QMessageBox msgBoxi (QMessageBox::Information, tr("Info"), "This operation will overwrite all the credentials on the device with the ones on the list.\n\nContinue?",0,this);
    msgBoxi.addButton(tr("&Ok"), QMessageBox::AcceptRole);
    msgBoxi.addButton(tr("&Cancel"), QMessageBox::RejectRole);

    if (msgBoxi.exec() == QMessageBox::RejectRole) {
        this->connectionTimer->start(1000);
        return;
    }


    if (ui->pinEdit->text().length() != 4) {
       QMessageBox msgBox (QMessageBox::Information, tr("Info"), "Please introduce the device PIN to decrypt credentials.",0,this);
       msgBox.addButton(tr("&Ok"), QMessageBox::AcceptRole);

       msgBox.exec();
       this->connectionTimer->start(1000);
       return;
    }


    Memtype_connect();

    if (!checkPINToDevice(ui->pinEdit->text().toInt())) {
       QMessageBox msgBox (QMessageBox::Information, tr("Info"), "The PIN is incorrect!",0,this);
       msgBox.addButton(tr("&Ok"), QMessageBox::AcceptRole);
       msgBox.exec();
       this->connectionTimer->start(1000);
       cleanPIN();
       return;
    }

    /* Allocate space */
    memtype_credential_t *cred_buff = (memtype_credential_t*)malloc(MEMTYPE_BUFFER_SIZE);

    this->writeSize = 0;
    int i = 0;
    while ((this->writeSize < MEMTYPE_BUFFER_SIZE) && (i < this->mCredentials.length())) {
        auto cred = this->mCredentials.at(i);
        this->writeSize += cred->Size();
        cred_buff[i].name = (char*)cred->name.c_str();
        cred_buff[i].user = (char*)cred->user.c_str();
        cred_buff[i].hop = (char*)cred->hop.c_str();
        cred_buff[i].pass = (char*)cred->password.c_str();
        cred_buff[i].submit = (char*)cred->submit.c_str();
        i++;
    }

    int clen = i;
    int buffsize = Memtype_credBuffSize(cred_buff,clen);
    uint8_t *buf = (uint8_t*)malloc(buffsize);
    Memtype_encrypt(cred_buff,clen,buf,buffsize,ui->pinEdit->text().toInt());

    Memtype_writeProgress(buf, buffsize, 0);
    free(buf);
    free(cred_buff);

    Memtype_disconnect();

    renderCredentials();

    QMessageBox msgBox (QMessageBox::Information, tr("Info"), "Credential write complete.",0,this);
    msgBox.addButton(tr("&Ok"), QMessageBox::AcceptRole);
    msgBox.exec();
    this->connectionTimer->start(1000);
    cleanPIN();
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

memtype_ret_t MainWindow::Memtype_writeProgress(const uint8_t * block, uint16_t len, uint16_t offset)
{
    QProgressDialog progress("Writting...", "&Cancel", 0, len, this);
    progress.show();

    memtype_ret_t ret;
    uint8_t *buff;
    uint8_t msg[8] = { 3, 0, 0, 0, 0, 0, 0, 0 };
    uint8_t l = sizeof(msg);
    uint16_t i;

    /* Prepare command for offset and size */
    msg[1] = offset & 0xFF;
    msg[2] = (offset >> 8) & 0xFF;
    msg[3] = len & 0xFF;
    msg[4] = (len >> 8) & 0xFF;

    /* Prepare Buffer to dynamic allocated memory */
    /* Allocate 8 bytes more than needed */
    buff = (uint8_t *) malloc(len + 8);

    if ((block != NULL) && (buff != NULL) && ((len + offset) < MEMTYPE_BUFFER_SIZE)) {
        /* Send CMD */
        QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
        ret = memtype_send(msg, &l);
        QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
        memcpy(buff, block, len);

        if (msg[0] == 3) {
            /* Send Data to write */
            for (i = 0; i < len; i += 8) {
                l = 8;
                QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
                memtype_send(&buff[i], &l);
                USB_WAIT_50ms();
                progress.setValue(i);
                QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
            }
            /* Compare buff and block to be equal */
            if (memcmp(buff, block, len) != 0) {
                ret = ERROR;
            }

        } else {
            ret = ERROR;
        }

    } else {
        ret = ERROR;
    }

    /* Free dynamic allocated memory */
    free(buff);
    progress.setValue(len);
    QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);

    return ret;
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

void MainWindow::setPinToDevice()
{
    this->connectionTimer->stop();
    if (!this->dev.present) {
        QMessageBox msgBox(QMessageBox::Warning, tr("Warning"), "No device found.", 0, this);
            msgBox.setDetailedText("To change the PIN from your MemType device it has to be connected to a USB port. If the device is connected check the documentation to ensure the installation is Ok and your user has access rights to the USB.");
            msgBox.addButton(tr("&Ok"), QMessageBox::AcceptRole);
        msgBox.exec();
        this->connectionTimer->start(1000);
        return;
    }

    if (memtypeLocked()) {
        QMessageBox msgBox(QMessageBox::Warning, tr("Warning"), "Device Locked.", 0, this);
            msgBox.setDetailedText("You must unlock your MemType device first by entering the P.I.N. using the joystick.");
            msgBox.addButton(tr("&Ok"), QMessageBox::AcceptRole);
        msgBox.exec();
        this->connectionTimer->start(1000);
        return;
    }

    if (ui->pinEdit->text().length() != 4) {
       QMessageBox msgBox (QMessageBox::Information, tr("Info"), "Please introduce the device PIN to continue.",0,this);
       msgBox.addButton(tr("&Ok"), QMessageBox::AcceptRole);

       msgBox.exec();
       this->connectionTimer->start(1000);
       return;
    }

    bool ok;
    QString new_pin1 = QInputDialog::getText(this, tr("Change PIN"),
                                         tr("New PIN:"), QLineEdit::Password,
                                         "", &ok);
    if (!ok) {
       this->connectionTimer->start(1000);
       return;
    }
    /* Validate new PIN */
    bool intOK;
    int new_pin1_int = new_pin1.toInt(&intOK);
    if (new_pin1.length() != 4 || !intOK || new_pin1_int < 0 || new_pin1_int > 9999) {
       QMessageBox msgBox (QMessageBox::Information, tr("Info"), "PIN must be a 4 digit number from 0000 to 9999.",0,this);
       msgBox.addButton(tr("&Ok"), QMessageBox::AcceptRole);
       msgBox.exec();

       this->connectionTimer->start(1000);
       return;
    }

    QString new_pin2 = QInputDialog::getText(this, tr("Confirm PIN"),
                                         tr("Repeat New PIN:"), QLineEdit::Password,
                                         "", &ok);
    if (!ok) {
       this->connectionTimer->start(1000);
       return;
    }
    /* Validate new PIN */
    int new_pin2_int = new_pin2.toInt(&intOK);
    if (new_pin2.length() != 4 || !intOK || new_pin2_int < 0 || new_pin2_int > 9999) {
       QMessageBox msgBox (QMessageBox::Information, tr("Info"), "PIN must be a 4 digit number from 0000 to 9999.",0,this);
       msgBox.addButton(tr("&Ok"), QMessageBox::AcceptRole);
       msgBox.exec();
       this->connectionTimer->start(1000);
       return;
    }

    if (new_pin1_int != new_pin2_int) {
       QMessageBox msgBox (QMessageBox::Information, tr("Info"), "Error confirming the PIN.",0,this);
       msgBox.addButton(tr("&Ok"), QMessageBox::AcceptRole);
       msgBox.exec();
       this->connectionTimer->start(1000);
       return;
    }

     if (ui->pinEdit->text().toInt() == new_pin1_int) {
       QMessageBox msgBox (QMessageBox::Information, tr("Info"), "The New PIN is the same as the current one. No action needed.",0,this);
       msgBox.addButton(tr("&Ok"), QMessageBox::AcceptRole);
       msgBox.exec();
       this->connectionTimer->start(1000);
       return;
    }

    Memtype_connect();

    if (!checkPINToDevice(ui->pinEdit->text().toInt())) {
       QMessageBox msgBox (QMessageBox::Information, tr("Info"), "The PIN is incorrect!",0,this);
       msgBox.addButton(tr("&Ok"), QMessageBox::AcceptRole);
       msgBox.exec();
       this->connectionTimer->start(1000);
       cleanPIN();
       return;
    }


    /* read all the credentials from the device and decrypt */
    uint8_t *cred_buff = (uint8_t*)malloc(MEMTYPE_BUFFER_SIZE);
    Memtype_readProgress(cred_buff, MEMTYPE_BUFFER_SIZE, 0);
    int clen = Memtype_credLen(cred_buff, MEMTYPE_BUFFER_SIZE);
    memtype_credential_t *list = (memtype_credential_t *) malloc(sizeof(memtype_credential_t) * clen);

    Memtype_decrypt(list, clen, cred_buff, MEMTYPE_BUFFER_SIZE, ui->pinEdit->text().toInt());

   /* encrypt and write back to device */

    int buffsize = Memtype_credBuffSize(list,clen);
    uint8_t *ebuf = (uint8_t*)malloc(buffsize);
    Memtype_encrypt(list,clen,ebuf,buffsize,new_pin1_int);

    Memtype_writeProgress(ebuf,buffsize, 0);
    uint8_t hash[MEMTYPE_HASH_LENGTH];

    Memtype_pinToHash(new_pin1_int,hash);
    Memtype_write_pin_hash(hash);
    free(list);
    free(cred_buff);

    Memtype_disconnect();

    ui->pinEdit->setText(new_pin2);

    QMessageBox msgBox (QMessageBox::Information, tr("Info"), "Set PIN complete.",0,this);
    msgBox.addButton(tr("&Ok"), QMessageBox::AcceptRole);
    msgBox.exec();
    this->connectionTimer->start(1000);
    cleanPIN();
}

void MainWindow::showVersion()
{
    auto versionWindow = new VersionWindow();
    versionWindow->show();
}

void MainWindow::cleanPIN()
{
   if (! ui->rememberButton->isChecked()) {
       ui->pinEdit->setText("");
   }
}

// Check that the entered PIN and the Device PIN are the same
bool MainWindow::checkPINToDevice(uint16_t pin)
{
    uint8_t hash_read[MEMTYPE_HASH_LENGTH];
    uint8_t hash_entered[MEMTYPE_HASH_LENGTH];

    Memtype_read_pin_hash(hash_read);
    Memtype_pinToHash(pin,hash_entered);
    return (memcmp(hash_read, hash_entered, MEMTYPE_HASH_LENGTH) == 0);
}
