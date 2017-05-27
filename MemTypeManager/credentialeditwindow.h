#ifndef CREDENTIALEDITWINDOW_H
#define CREDENTIALEDITWINDOW_H

#include <QMainWindow>
#include "credential.h"

namespace Ui {
class CredentialEditWindow;
}

class CredentialEditWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit CredentialEditWindow(QWidget *parent = 0);
    ~CredentialEditWindow();

    void setCredential(Credential *credential);

public slots:
    void addDelay();
signals:
    void accepted();

private:
    Credential* cred;
    Ui::CredentialEditWindow *ui;
    void saveCredential();
};

#endif // CREDENTIALEDITWINDOW_H
