#ifndef DEVICE_H
#define DEVICE_H

#include "memtype_api.h"

class device {

private:
    bool present;
    device_info_t info;
    memtype_locked_t locked;

public:
    device_info_t getInfo();
};

#endif // DEVICE_H
