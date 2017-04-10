#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "credential.h"
#include "credentialwidget.h"
#include "credentialeditwindow.h"
#include <QApplication>
#include <QDebug>
#include <QLabel>
#include <QString>
#include <QFileDialog>

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

private:
    Ui::MainWindow *ui;
    QVector<Credential*> mCredentials;
    CredentialEditWindow* credWindow;

    void renderCredentials();
    void editCredential(Credential *credential);
    void moveUpCredential(Credential *credential);
    void moveDownCredential(Credential *credential);
    void loadLayout();
    void exportCredentials();
    void importCredentials();
};

#endif // MAINWINDOW_H
