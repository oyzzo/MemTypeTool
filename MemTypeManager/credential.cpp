#include "credential.h"

Credential::Credential()
{
    this->name = "";
    this->user = "";
    this->hop = "\\t";
    this->password = "";
    this->submit = "\\n";
    this->fits = false;
}

uint16_t Credential::Size()
{
    uint16_t size;

    memtype_credential_t cred;
    cred.name = (char*)this->name.c_str();
    cred.user = (char*)this->user.c_str();
    cred.hop = (char*)this->hop.c_str();
    cred.pass = (char*)this->password.c_str();
    cred.submit = (char*)this->submit.c_str();

    size = Memtype_credBuffSize(&cred, 1);

    return size;
}
