//
// Created by Administrator on 2020/2/22 0022.
//

//#include <iostream>
#include "SerialMonitor.h"
#include "dbg_log.h"

using namespace std;
#ifdef DEBUG_PC_MONITOR
int32_t SerialMonitor::Open(FM_CONTEXT_t const *fm_ctx)
{
    cout << __FUNCTION__ << endl;
    //创建socket对象
    m_sockfd = socket(AF_INET,SOCK_DGRAM,0);

    //创建网络通信对象
    m_addr.sin_family = AF_INET;
    m_addr.sin_port = htons(1115);
    m_addr.sin_addr.s_addr = inet_addr("120.41.153.229");

    return 0;
}

void SerialMonitor::Close()
{
    cout << __FUNCTION__ << endl;
    close(m_sockfd);
}

void Smdump_hex(uint8_t const *buf, uint32_t len)
{
    int i;

    for(i=0; i<len; i++)
    {
        LOG_RAW("%02x ", buf[i]);
    }
    LOG_RAW("\n");
}

int32_t SerialMonitor::SendData(void const *dat, uint32_t len)
{
    //cout << __FUNCTION__ << endl;
    Smdump_hex((uint8_t const *)dat, len);
    sendto(m_sockfd, dat, len, 0, (struct sockaddr*)&m_addr, sizeof(m_addr));
    return 0;
}

int32_t SerialMonitor::ReceiveData(void *recv_data, uint32_t len, uint32_t time_out)
{
    //cout << __FUNCTION__ << endl;
    socklen_t addr_len = sizeof(m_addr);
    return recvfrom(m_sockfd, recv_data, len, 0,(struct sockaddr*)&m_addr, &addr_len);
}
#endif