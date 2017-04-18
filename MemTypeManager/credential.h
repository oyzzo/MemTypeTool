#ifndef CREDENTIAL_H
#define CREDENTIAL_H

#include <string>
#include "memtype_api.h"

using namespace std;

class Credential
{
public:
    // Attributes
    string name;
    string user;
    string hop;
    string password;
    string submit;
    bool fits; // Is the credential writted into device?

    Credential();
    int Size();
};

#endif // CREDENTIAL_H
