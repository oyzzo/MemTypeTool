#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "credentialwidget.h"
#include "statuswidget.h"
#include "credential.h"
#include "device.h"
#include "credentialeditwindow.h"
#include "memtype_api.h"
#include "noekeon_api.h"
#include <QApplication>
#include <QDebug>
#include <QLabel>
#include <QString>
#include <QFileDialog>
#include <QTimer>
#include <QMessageBox>
#include <QProgressDialog>
#include <QFuture>
#include <QtConcurrent/qtconcurrentrun.h>
#include <QIntValidator>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void addCredential(void);
    void deleteCredential(Credential *cred);   
    bool updateConnection();
    void validatePIN();
    void showPin();
    void hidePin();
    void cleanCredentialList();
private:
    Ui::MainWindow *ui;
    QVector<Credential *> mCredentials;
    CredentialEditWindow* credWindow;
    device dev;
    statuswidget *stWidget;
    QTimer *connectionTimer;

    void renderCredentials();
    void editCredential(Credential *credential);
    void moveUpCredential(Credential *credential);
    void moveDownCredential(Credential *credential);
    void loadLayout();
    void exportCredentials();
    void importCredentials();
    bool memtypeLocked();
    void readFromDevice();
    memtype_ret_t Memtype_readProgress(uint8_t *block, uint16_t len, uint16_t offset);
};

#endif // MAINWINDOW_H
