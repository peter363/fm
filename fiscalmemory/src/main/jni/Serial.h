//
// Created by Administrator on 2020/2/22 0022.
//

#ifndef SERIALDEMO_SERIAL_H
#define SERIALDEMO_SERIAL_H

#include "SerialHal.h"
#include "FmContext.h"

class Serial: public SerialHal {
private:
    int32_t m_Serialfd;
    int32_t SerialDirect_Open();
    int32_t SerialDirect_Write(void const *buf, uint32_t wr_len);
    int32_t SerialDirect_Read(void *buf, uint32_t rd_len);
    void SerialDirect_Close();
public:
    Serial():SerialHal() { m_Serialfd = -1; };
    ~Serial() {};
    FM_CONTEXT_t const*m_fm_ctx;

    int32_t Open(FM_CONTEXT_t const *fm_ctx);
    void Close();
    int32_t SendData(void const *dat, uint32_t len);
    int32_t ReceiveData(void *recv_data, uint32_t len, uint32_t time_out);
};



#endif //SERIALDEMO_SERIAL_H
