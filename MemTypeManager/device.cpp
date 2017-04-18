#include "device.h"

device_info_t device::getInfo(void)
{
    memtype_ret_t err;

    Memtype_connect();

    err = Memtype_info(&this->info);

    //TODO TREAT ERROR
    Memtype_disconnect();
}
