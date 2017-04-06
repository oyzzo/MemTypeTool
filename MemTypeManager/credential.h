#ifndef CREDENTIAL_H
#define CREDENTIAL_H

#include <string>

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

    Credential();
};

#endif // CREDENTIAL_H
