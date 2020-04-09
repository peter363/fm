//
// Created by Administrator on 2020/2/23 0023.
//

#ifndef SERIALDEMO_PROTOCOLPARSER_H
#define SERIALDEMO_PROTOCOLPARSER_H

#include <stdint.h>
#include "fmhead.h"
#include "SerialHal.h"
#include "FmError.h"
#include "FmContext.h"


class ProtocolParser {
private:
    uint8_t m_buf[1200];    
    bool Inited = false;
    uint32_t m_timeout_ms = 300;    //timeout time
    SerialHal * m_hal;
    //SerialHal hal;
    uint32_t PacketHeader(void * container, uint8_t cmd, uint32_t addr, void const *buf,  uint16_t buf_len);
public:
    int32_t Open(FM_CONTEXT_t const *fm_ctx);
    void Close();
    int32_t GetFmInfo(str_fmInfo *info);
    int32_t ReadData(uint32_t addr, void * rd_buf, uint32_t buf_size);
    int32_t WriteData(uint32_t addr, void const* wr_buf, uint32_t wr_len);
    int32_t Erase(uint32_t addr);
};



#endif //SERIALDEMO_PROTOCOLPARSER_H
