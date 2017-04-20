#ifndef DEVICE_H
#define DEVICE_H

#include "memtype_api.h"

class device {

public:
    bool present;
    memtype_info_t info;
    memtype_locked_t locked;
    bool talking;

    memtype_info_t getInfo();
};

#endif // DEVICE_H
