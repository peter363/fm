//
// Created by Administrator on 2020/2/22 0022.
//

#ifndef SERIALDEMO_SERIALMONITOR_H
#define SERIALDEMO_SERIALMONITOR_H

//socket udp 客户端
#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<arpa/inet.h>
#include "SerialHal.h"
#include "FmContext.h"

#ifdef DEBUG_PC_MONITOR
class SerialMonitor: public SerialHal {
//class SerialMonitor  {
private:
    int m_sockfd;
    struct sockaddr_in m_addr;
public:
    SerialMonitor():SerialHal() {};
    ~SerialMonitor() {};
    int32_t Open(FM_CONTEXT_t const *fm_ctx);
    void Close();
    int32_t SendData(void const *dat, uint32_t len);
    int32_t ReceiveData(void *recv_data, uint32_t len, uint32_t time_out);
};
#endif


#endif //SERIALDEMO_SERIALMONITOR_H
