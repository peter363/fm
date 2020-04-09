//
// Created by Administrator on 2020/2/25 0025.
//

#ifndef SERIALDEMO_CRC_H
#define SERIALDEMO_CRC_H

#include <stdint.h>

class Crc {
private:
    static const uint16_t m_fcstab[256];
public:
    static uint16_t Crc16(uint16_t fcs, void const *buf, uint16_t len)
    {
        uint8_t *pBuf;
        pBuf = (uint8_t *)buf;
        while (len--)
        {
            fcs = (fcs << 8) ^ Crc::m_fcstab[(fcs >> 8) ^ *pBuf++];
        }
        return (fcs);
    }    
};



#endif //SERIALDEMO_CRC_H
