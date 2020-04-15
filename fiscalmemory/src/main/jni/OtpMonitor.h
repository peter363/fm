//
// Created by Administrator on 2020/2/23 0023.
//

#ifndef SERIALDEMO_OTPMONITOR_H
#define SERIALDEMO_OTPMONITOR_H

#include <unistd.h>
#include <pthread.h>
//#include <jni.h>
#include <errno.h>
#include "ProtocolParser.h"
#include "fmhead.h"
#include "FmError.h"
#include "FmContext.h"

class OtpMonitor {
private:
    pthread_mutex_t  g_mutex;
    int32_t mutex_lock();
    int32_t mutex_unlock();
    static bool m_OpenFlag;
    str_fmInfo m_FmInfo;
    uint8_t *m_map;
    ProtocolParser m_dev;
    void OtpMonitor_SyncMap();
public:
    OtpMonitor();
    ~OtpMonitor();
    int32_t Open(FM_CONTEXT_t const *fm_ctx);
    void Close();
    bool IsInited();
    str_fmInfo GetFmInfo();
    void OtpMonitor_SyncMapByArea(uint32_t addr, uint32_t len);
    int32_t ReadByte(uint32_t addr, uint8_t &rd_byte);
    int32_t ReadData(uint32_t addr, void * rd_buf, uint32_t rd_len);
    int32_t WriteData(uint32_t addr, void const* wr_buf, uint32_t wr_len);
    int32_t EraseAll();
    void DumpMap();
    uint8_t const *GetMap();
};



#endif //SERIALDEMO_OTPMONITOR_H
