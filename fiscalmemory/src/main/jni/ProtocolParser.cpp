#define TAG "PROTOCOL"

//
// Created by Administrator on 2020/2/23 0023.
//
#if 1
#include <string.h>
#include <errno.h>
#include "ProtocolParser.h"
#include "SerialMonitor.h"
#include "Serial.h"
#include "Crc.h"
#include "dbg_log.h"
#include "FmError.h"


static uint8_t const GetFmInfoCmd[] =
{
    0x02, 0x00,
    0x00, 0x0A,
    0xFF, 0xF5,
    0x0D, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x56, 0x64
};


int32_t ProtocolParser::Open(FM_CONTEXT_t const *fm_ctx)
{
    int32_t ret;
#ifdef DEBUG_PC_MONITOR    
    LOGD("%s\n", __FUNCTION__);
    m_hal = new SerialMonitor();
#else
    m_hal = new Serial();
#endif
    LOGD("%s\n", __FUNCTION__);
    ret = m_hal->Open(fm_ctx);
    //m_timeout_ms = timeout_ms;
    if(ret < 0)
    {
        delete m_hal;
        return ret;
    }
    Inited = true;
    return 0;
}

void ProtocolParser::Close()
{
    m_hal->Close();
    delete m_hal;
    Inited = false;
    m_hal = nullptr;
}

void dump_hex(uint8_t const *buf, uint32_t len)
{
    int i;

    for(i=0; i<len; i++)
    {
        LOG_RAW("%02x ", buf[i]);
    }
    LOG_RAW("\n");
}

static bool CheckAckHeadValid(str_fmAck *p_ack, uint32_t len) 
{
//    LOGD("len %u, %lu %x %x %x\n", len, ntohs(p_ack->len) + sizeof(str_fmAck), p_ack->len, ~p_ack->rlen, p_ack->len^p_ack->rlen);
    if((p_ack->ack == FM_ACK) && (0xFFFF == (p_ack->len ^ p_ack->rlen)) 
    && (len == (ntohs(p_ack->len) + sizeof(str_fmAck))))
    {
        //TODO CRC CHECK
        //uint16_t crc = htons(Crc16(0, p_ack->ndata, len));
        //if(crc == ntohs(p_ack->ndata[]))
        p_ack->len = ntohs(p_ack->len);
        return true;
    }

    return false;
}

int32_t ProtocolParser::GetFmInfo(str_fmInfo *info)
{
    int ret;
    str_fmAck *p_ack;
    str_fmInfo *p_getinfo;
    // uint16_t crc;

    // crc = Crc::Crc16(0, &GetFmInfoCmd[6], sizeof(GetFmInfoCmd) - 6);
    // LOGD("crc is %x\n", crc);
    // memcpy(m_buf, GetFmInfoCmd, sizeof(GetFmInfoCmd));
    // *(uint16_t *)&(m_buf[sizeof(GetFmInfoCmd)]) = htons(crc);
    ret = m_hal->SendData(GetFmInfoCmd, sizeof(GetFmInfoCmd));
    if(ret < 0)
    {
        LOGE("%s send err %d\n", __FUNCTION__, ret);
        return FM_HARDWARE_ERR;
    }

    ret = m_hal->ReceiveData(m_buf, sizeof(m_buf), m_timeout_ms);
    LOGD("%s ret = %d", __FUNCTION__, ret);
    if(ret < 0)
    {
        LOGE("%s recv err %d\n", __FUNCTION__, ret);
//        dump_hex(m_buf, ret);
        return FM_HARDWARE_ERR;
    }
    p_ack = (str_fmAck *)m_buf;
    if(CheckAckHeadValid(p_ack, ret))
    {
        if(p_ack->errcode == 1)
        {
            p_getinfo = (str_fmInfo *)p_ack->ndata;
            memcpy(info->fm_type, p_getinfo->fm_type, sizeof(info->fm_type)); 
            info->fm_size = ntohl(p_getinfo->fm_size);    
            info->fm_version = ntohl(p_getinfo->fm_version);
            info->fm_single_read_size = ntohl(p_getinfo->fm_single_read_size);
            info->fm_single_write_size = ntohl(p_getinfo->fm_single_write_size);
            info->fm_erase_size = ntohl(p_getinfo->fm_erase_size);
            info->fm_00 = ntohl(p_getinfo->fm_00);
            info->fm_01 = ntohl(p_getinfo->fm_01);
            info->fm_02 = ntohl(p_getinfo->fm_02);
            info->fm_03 = ntohl(p_getinfo->fm_03);
            LOGD("fm_type(%s) fm_size(%u) fm_version(%u), fm_single_read_size(0x%x) fm_single_write_size(0x%x) fm_erase_size(0x%x)\n",
                info->fm_type, info->fm_size, info->fm_version, info->fm_single_read_size, info->fm_single_write_size,
                info->fm_erase_size);     
        }else
        {
            LOGE("%s recv err %d errcode(%u)\n", __FUNCTION__, ret, p_ack->errcode);
            return -p_ack->errcode;
        }
    }else
    {
        LOGE("%s recv err %d\n", __FUNCTION__, ret);
        //dump_hex(m_buf, ret);
        return FM_INVALID_CRC;
    }
    return ret;
}

uint32_t ProtocolParser::PacketHeader(void * container, uint8_t cmd, uint32_t addr, void const *buf,  uint16_t buf_len)
{
    str_fmsend * head = (str_fmsend *)container;
    uint16_t tot_len;
    uint16_t crc = 0;

    head->stx = FM_STX;
    head->fix0 = 0;
    //
    head->cmd = cmd;
    head->dev = FM_DEV_ROM;
    head->index = 0;
    head->alt = htonl(addr);
    //
    //LOGD("buf = %p, len = %u\n", buf, buf_len);
    if(buf != nullptr && buf_len > 0)
    {
        memcpy(head->ndata, buf, buf_len);        
    }else
    {
        buf_len = 0;
    }
    crc = Crc::Crc16(0, &(head->cmd), buf_len + 8);
    //LOGD("Calculate crc is %x\n", crc);
    //tot len is cmdheader len add buf_len and crc
    tot_len = sizeof(str_fmsend) - sizeof(str_fmhead) + buf_len + 2;
    head->len = htons(tot_len);
    head->rlen = ~(head->len);

    //TODO CRC
    *(uint16_t *)&(head->ndata[buf_len]) = htons(crc); 

    return tot_len + sizeof(str_fmhead);
}


int32_t ProtocolParser::ReadData(uint32_t addr, void * rd_buf, uint32_t buf_size)
{
    str_fmAck *p_ack;
    uint32_t len;
    int ret;    

    len = PacketHeader(m_buf, FM_CMD_READ, addr, nullptr, 0);
    LOGD("%s %x", __FUNCTION__, addr);
    ret = m_hal->SendData(m_buf, len);
    if(ret < 0)
    {
        LOGE("%s err %d\n", __FUNCTION__, ret);
        return FM_HARDWARE_ERR;
    }

    ret = m_hal->ReceiveData(m_buf, sizeof(m_buf), m_timeout_ms);
    if(ret < 0)
    {
        LOGE("%s err %d\n", __FUNCTION__, ret);  
        return FM_HARDWARE_ERR;
    }

    p_ack = (str_fmAck *)m_buf;
    if(!CheckAckHeadValid(p_ack, ret))
    {
        LOGE("%s recv err CheckAckHeadValid %d\n", __FUNCTION__, ret);
        //dump_hex(m_buf, ret);
        return FM_INVALID_CRC;
    }
    else if(p_ack->errcode > 1)
    {
        LOGE("%s recv err %d errcode(%u)\n", __FUNCTION__, ret, p_ack->errcode);
        return -p_ack->errcode;
    }

    len = (p_ack->len - 2) > buf_size ? buf_size : (p_ack->len - 2);
    memcpy(rd_buf, p_ack->ndata, len);

    return len;
}

int32_t ProtocolParser::WriteData(uint32_t addr, void const* wr_buf, uint32_t wr_len)
{
    str_fmAck *p_ack;
    uint32_t len;
    int ret;    

    len = PacketHeader(m_buf, FM_CMD_WRITE, addr, wr_buf, wr_len);

    //dump_hex(m_buf, len);
    ret = m_hal->SendData(m_buf, len);
    if(ret < 0)
    {
        LOGE("%s err %d\n", __FUNCTION__, ret);
        return FM_HARDWARE_ERR;
    }

    ret = m_hal->ReceiveData(m_buf, sizeof(m_buf), m_timeout_ms);
    if(ret < 0)
    {
        LOGE("%s err %d\n", __FUNCTION__, ret);  
        return FM_HARDWARE_ERR;
    }

    p_ack = (str_fmAck *)m_buf;
    if(p_ack->ack != FM_ACK)
    {
        LOGE("%s recv err %d\n", __FUNCTION__, ret);
        //dump_hex(m_buf, ret);
        return FM_INVALID_CRC;
    }else if(p_ack->errcode > 1)
    {
        LOGE("%s recv err %d errcode(%u)\n", __FUNCTION__, ret, p_ack->errcode);
        return -p_ack->errcode;
    }

    return FM_SUCCESS;
}

int32_t ProtocolParser::Erase(uint32_t addr)
{
    str_fmAck *p_ack;
    uint32_t len;
    int ret;    

    len = PacketHeader(m_buf, FM_CMD_ERASE, addr, NULL, 0);

    ret = m_hal->SendData(m_buf, len);
    if(ret < 0)
    {
        LOGE("%s err %d\n", __FUNCTION__, ret);
        return FM_HARDWARE_ERR;
    }

    ret = m_hal->ReceiveData(m_buf, sizeof(m_buf), m_timeout_ms);
    if(ret < 0)
    {
        LOGE("%s err %d\n", __FUNCTION__, ret);  
        return FM_HARDWARE_ERR;
    }

    p_ack = (str_fmAck *)m_buf;
    if(p_ack->ack != FM_ACK)
    {
        LOGE("%s recv err %d\n", __FUNCTION__, ret);
        //dump_hex(m_buf, ret);
        return FM_INVALID_CRC;
    }else if(p_ack->errcode > 1)
    {
        LOGE("%s recv err %d errcode(%u)\n", __FUNCTION__, ret, p_ack->errcode);
        return -p_ack->errcode;
    }

    return FM_SUCCESS;
}

#endif