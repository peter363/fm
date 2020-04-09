//
// Created by Administrator on 2020/2/23 0023.
//
#define TAG "OTP"

#include <stdlib.h>
#include <string.h>
#include "OtpMonitor.h"
#include "dbg_log.h"

#define TEST_FLASH_SIZE 40284

bool OtpMonitor::m_OpenFlag = false;

OtpMonitor::OtpMonitor()
{
    if (0 != pthread_mutex_init(&g_mutex, NULL)) {
		//异常
		// jclass exceptionClazz = env->FindClass("java/lang/RuntimeException");
		// //抛出
		// env->ThrowNew(exceptionClazz, "Unable to init mutex--");
	}
}

OtpMonitor::~OtpMonitor()
{
	//释放互斥量
	if (0 != pthread_mutex_destroy(&g_mutex)) {
		//异常
        LOGE("%s Err", __FUNCTION__);
		// jclass exceptionClazz = env->FindClass("java/lang/RuntimeException");
		// //抛出
		// env->ThrowNew(exceptionClazz, "Unable to destroy mutex--");
	}
}

int32_t OtpMonitor::mutex_lock()
{
    	//lock
	if (0 != pthread_mutex_lock(&g_mutex)) {
        LOGE("%s Err", __FUNCTION__);
		//异常
		// jclass exceptionClazz = env->FindClass("java/lang/RuntimeException");
		// //抛出
		// env->ThrowNew(exceptionClazz, "Unable to lock mutex--");
		return -EIO;
	}
	return 0;
}


int32_t OtpMonitor::mutex_unlock()
{
	//unlock
	if (0 != pthread_mutex_unlock(&g_mutex)) {
        LOGE("%s Err", __FUNCTION__);
		//异常
		// jclass exceptionClazz = env->FindClass("java/lang/RuntimeException");
		// //抛出
		// env->ThrowNew(exceptionClazz, "Unable to unlock mutex--");
        return -EIO;
	}
	return 0;
}

void OtpMonitor::OtpMonitor_SyncMap()
{
    //read all data
    for(uint32_t i=0; i<TEST_FLASH_SIZE/*m_FmInfo.fm_size*/; i+=m_FmInfo.fm_single_read_size)
    {
        m_dev.ReadData(i, &m_map[i], m_FmInfo.fm_single_read_size);
    }
}

void OtpMonitor::OtpMonitor_SyncMapByArea(uint32_t addr, uint32_t len)
{
//    uint32_t align_addr_begin = addr / m_FmInfo.fm_single_read_size * m_FmInfo.fm_single_read_size;
//    uint32_t align_addr_end = (addr + len) / m_FmInfo.fm_single_read_size * m_FmInfo.fm_single_read_size;
//    //read all data
//    LOGD("%s [%x %u] [%x - %x]", __FUNCTION__,  addr, len, align_addr_begin, align_addr_end);
//    for(uint32_t i=align_addr_begin; i<=align_addr_end; i+=m_FmInfo.fm_single_read_size)
//    {
//        LOGD("Read %x", i);
//        m_dev.ReadData(i, &m_map[i], m_FmInfo.fm_single_read_size);
//    }
}

int32_t OtpMonitor::Open(FM_CONTEXT_t const *fm_ctx)
{
    int32_t ret;
    uint32_t i;
    //std::lock_guard<std::mutex> lock(g_mutex);

    if(m_OpenFlag)
    {
        LOGE("%s FM_BUSY\n", __FUNCTION__);
        return FM_BUSY;
    }

    mutex_lock();
    ret = m_dev.Open(fm_ctx);
    if(ret < 0)
    {
        LOGE("%s %d FM_HARDWARE_ERR err(%d)\n", __FUNCTION__, __LINE__, ret);
        mutex_unlock();
        return FM_HARDWARE_ERR;
    }

    ret = m_dev.GetFmInfo(&m_FmInfo);
    if(ret < 0)
    {
        LOGE("%s %d FM_HARDWARE_ERR err(%d)\n", __FUNCTION__, __LINE__, ret);
        m_dev.Close();
        mutex_unlock();
        return FM_HARDWARE_ERR;
    }

    if(m_FmInfo.fm_size == 0 || m_FmInfo.fm_single_read_size == 0 || m_FmInfo.fm_single_write_size == 0  || m_FmInfo.fm_erase_size == 0)
    {
        LOGE("%s %d FM_HARDWARE_ERR\n", __FUNCTION__, __LINE__);
        LOGE("fm_type(%s) fm_size(%u) fm_version(%u), fm_single_read_size(0x%x) fm_single_write_size(0x%x) fm_erase_size(0x%x)\n",
                m_FmInfo.fm_type, m_FmInfo.fm_size, m_FmInfo.fm_version, m_FmInfo.fm_single_read_size, m_FmInfo.fm_single_write_size,
                m_FmInfo.fm_erase_size); 
        m_dev.Close();
        mutex_unlock();
        return FM_HARDWARE_ERR;
    }

    //malloc map
    m_map = (uint8_t *)malloc(m_FmInfo.fm_size);
    if(m_map == nullptr)
    {
        m_dev.Close();
        mutex_unlock();
        return FM_HARDWARE_ERR;
    }

    OtpMonitor_SyncMap();
    m_OpenFlag = true;

    mutex_unlock();
    return FM_SUCCESS;
}

void OtpMonitor::DumpMap()
{
    if(m_map)
    {
        LOGI("DumpMap:\n");
        for(int i=0; i<m_FmInfo.fm_size; i++)
        {
            LOG_RAW("%02x ", m_map[i]);
        }
        LOG_RAW("\n");
    }
    else
    {
        LOGE("DumpMap is Empty\n");
    }
}

void OtpMonitor::Close()
{
    //std::lock_guard<std::mutex> lock(g_mutex);

    if(!m_OpenFlag)
    {
        return;
    }

    mutex_lock();
    if(m_map)
    {
        free(m_map);
        m_map = nullptr;
    }
    m_dev.Close();
    m_OpenFlag = false;
    mutex_unlock();
}

str_fmInfo OtpMonitor::GetFmInfo()
{
    return m_FmInfo;
}

int32_t OtpMonitor::ReadByte(uint32_t addr, uint8_t &rd_byte) 
{
    if(!m_OpenFlag)
    {
        LOGE("%s Not Open", __FUNCTION__);
        return FM_INVALID_IO;
    }

    if(addr >= m_FmInfo.fm_size) 
    {
        LOGE("%s FM_INVALID_ADDR\n", __FUNCTION__);
        return FM_INVALID_ADDR;
    }

    OtpMonitor_SyncMapByArea(addr, 1);
    rd_byte = m_map[addr];

    return 1;
}

int32_t OtpMonitor::ReadData(uint32_t addr, void * rd_buf, uint32_t rd_len)
{
    if(!m_OpenFlag)
    {
        return FM_INVALID_IO;
    }

    if((addr >= m_FmInfo.fm_size) || ((addr + rd_len) > m_FmInfo.fm_size))  
    {
        LOGE("%s FM_INVALID_ADDR\n", __FUNCTION__);
        return FM_INVALID_ADDR;
    }

    //std::lock_guard<std::mutex> lock(g_mutex);
    //check input
    mutex_lock();
    OtpMonitor_SyncMapByArea(addr, rd_len);
    memcpy(rd_buf, &m_map[addr], rd_len);
    mutex_unlock();
    return rd_len;
}

int32_t OtpMonitor::WriteData(uint32_t addr, void const* wr_buf, uint32_t wr_len)
{
    int ret;
    
    if(!m_OpenFlag)
    {
        return FM_INVALID_IO;
    }

    if((addr >= m_FmInfo.fm_size) || ((addr + wr_len) > m_FmInfo.fm_size)) 
    {
        LOGE("%s addr(%x) wr_len(%x) size(%x) FM_INVALID_ADDR\n", __FUNCTION__, addr, wr_len, m_FmInfo.fm_size);
        return FM_INVALID_ADDR;
    }

    //std::lock_guard<std::mutex> lock(g_mutex);
    //check input

    LOGD("write %x %s %u\n", addr, (char const *)wr_buf, wr_len);

    uint32_t write_addr = addr;
    uint32_t write_len = wr_len;
    uint32_t align_addr_end = addr/m_FmInfo.fm_single_write_size*m_FmInfo.fm_single_write_size + m_FmInfo.fm_single_write_size;
    LOGD("write_addr %x, write_len %u, align_addr_end %x\n", write_addr, write_len, align_addr_end);
    mutex_lock();
    OtpMonitor_SyncMapByArea(addr, wr_len);
    //TODO: NEED TO CHECK WRITE AREA IS EMPTY
    while(write_len > 0)
    {
        uint32_t len;
        
        if((write_addr + write_len) > align_addr_end)
        {
            len = align_addr_end - write_addr;
        }
        else
        {
            len = write_len;
        }
        LOGD("write to write_addr %x, len %u\n", write_addr, len);
        //sync to fm
        ret = m_dev.WriteData(write_addr, &(((uint8_t const *)wr_buf)[write_addr - addr]), len);
        if(ret < 0)
        {
            LOGE("%s err(%d)\n", __FUNCTION__, ret);
            mutex_unlock();
            return ret;
        }
        //copy buf
        memcpy(&m_map[write_addr], &(((uint8_t const *)wr_buf)[write_addr - addr]), len); 
        write_addr += len;
        write_len  -= len;
        align_addr_end += m_FmInfo.fm_single_write_size;
    } 
    mutex_unlock();   
    return wr_len;
}

int32_t OtpMonitor::EraseAll()
{
    if(!m_OpenFlag)
    {
        return FM_INVALID_IO;
    }

    //std::lock_guard<std::mutex> lock(g_mutex);
    int32_t ret ;

    mutex_lock();
    for(uint32_t i=0; i<TEST_FLASH_SIZE; i+=m_FmInfo.fm_erase_size)
    {
        if((ret = m_dev.Erase(i)) != FM_SUCCESS)
        {
            mutex_unlock();
            return ret;
        }
    }

    //OtpMonitor_SyncMap();

    mutex_unlock();
    return FM_SUCCESS;
}

uint8_t const * OtpMonitor::GetMap()
{
    return m_map;
}

bool OtpMonitor::IsInited()
{
    return m_OpenFlag;
}


