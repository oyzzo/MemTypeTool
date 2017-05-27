#ifndef CREDENTIALWIDGET_H
#define CREDENTIALWIDGET_H

#include <QWidget>
#include "credential.h"

namespace Ui {
class credentialWidget;
}

class credentialWidget : public QWidget
{
    Q_OBJECT
public:
    Credential* cred;
    explicit credentialWidget(QWidget *parent = 0);
    ~credentialWidget();
    void emitDeleteCredential(void);
    void emitEditCredential();
    void setCredentialName(QString);

signals:
    void deleteCredential(Credential* cred);
    void editCredential(Credential* cred);
    void upCredential(Credential* cred);
    void downCredential(Credential* cred);

private:
    Ui::credentialWidget *ui;
    void emitUpCredential();
    void emitDownCredential();
};

#endif // CREDENTIALWIDGET_H
