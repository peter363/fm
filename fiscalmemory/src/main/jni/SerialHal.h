//
// Created by Administrator on 2020/2/22 0022.
//

#ifndef SERIALDEMO_SERIALHAL_H
#define SERIALDEMO_SERIALHAL_H

#include <stdint.h>
#include "FmContext.h"

class SerialHal {
private:
    char DevName[80];
public:
    SerialHal() {};
    virtual ~SerialHal() {};
    virtual int32_t Open(FM_CONTEXT_t const *fm_ctx) = 0;
    virtual void Close() = 0;
    virtual int32_t SendData(void const *dat, uint32_t len) = 0;
    virtual int32_t ReceiveData(void *recv_data, uint32_t len, uint32_t time_out) = 0;
};


#endif //SERIALDEMO_SERIALHAL_H
