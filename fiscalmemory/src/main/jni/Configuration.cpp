/*************************************************************
 
   Copyright (c) 2008 telos EDV Systementwicklung GmbH,
   Hamburg (Germany)

                    http://www.telos.de

   $Id: Configuration.cpp 3816 2010-10-01 08:37:13Z mbudde $

   This file is part of "FM080525A - Fiscal Cash Register".

 *************************************************************/
#define TAG "CONFIGURATION"

#include "Configuration.h"

extern "C"
{
//#include "stm32f10x_lib.h"
}

#include <stdint.h>
//#include <algorithm>
#include <cstring>
#include <cassert>
#include <cstddef>
#include "dbg_log.h"

#define min(a, b)     ((a) > (b) ? (b) : (a))

using namespace std;


// static member variables
//Configuration::Data  Configuration::m_data;
//const uint32_t       Configuration::ADDRESS_DATA_IN_FLASH = reinterpret_cast<uint32_t>(&Configuration::m_data);
const int            Configuration::Z_REPORT_ENTRY_SIZE;

#define FM_DATA_BASE_ADDRESS     ((Configuration::Data *)(0))
#define GET_UINT32_ADDRESS(addr)  ((uint32_t)((uint64_t)(addr)))

/***********************************************************************/
/*!
 *  Constructor
 */
/***********************************************************************/

Configuration::Configuration() {
    LOGD("%s", __FUNCTION__);
}

Configuration::~Configuration() {
    LOGD("%s", __FUNCTION__);
    m_otp.Close();
    m_data = nullptr;
}

bool Configuration::IsInited() {
    return m_otp.IsInited();
}

int32_t Configuration::Init(FM_CONTEXT_t const *fm_ctx) {
    LOGD("Sizeof DATA = %d", (int)sizeof(Configuration::Data));

    int32_t ret = m_otp.Open(fm_ctx);
    if (ret < 0) {
        return ret;
    }

    m_data = (Data const *) m_otp.GetMap();
    if (m_data == nullptr) {
        m_otp.Close();
        return -EIO;
    }
    // calculate number of used flash pages
    // m_flash_pages = sizeof (m_data) / FLASH_PAGE_SIZE;
    // if (sizeof (m_data) % FLASH_PAGE_SIZE != 0)
    //   m_flash_pages++;

    // select factory mode
    m_user_mode = false;
    // If this is the first instance of this class, which has been created after
    // the manufacturing of the module, set the configuration values to the defaults.
    if (m_data->m_cleared != CLEARED) {
        LOGD("%s %d", __FUNCTION__, __LINE__);
        ClearFlash();
    }

    // calculate values returned by GetNumberOfEntries()
    CalculateNumberOfEntries();

    return 0;
}


/***********************************************************************/
/*!
 *  Clear complete internal flash.
 */
/***********************************************************************/
void Configuration::ClearFlash() {
    uint32_t clear = CLEARED;
    m_otp.EraseAll();

    // set flash state to cleared
    WriteFlash(GET_UINT32_ADDRESS((&(FM_DATA_BASE_ADDRESS->m_cleared))),
               reinterpret_cast<const uint8_t *>(&clear),
               sizeof(clear));
    // update values returned by GetNumberOfEntries()
    m_number_of_entries_factory_mode = 0;
    m_number_of_entries_user_mode = 0;
}


/***********************************************************************/
/*!
 *  Writes the data to the internal flash.
 *
 *   \param addr      start address in the flash memory
 *   \param data      data to be written
 *   \param data_len  number of data bytes to be written
 */
/***********************************************************************/
int32_t Configuration::WriteFlash(uint32_t addr, const uint8_t *data, int data_len) {
    LOGD("%s %8s addr = %x\n", __FUNCTION__, data, addr);
    return m_otp.WriteData(addr, data, data_len);
}


/***********************************************************************/
/*!
 *  Writes the string to the internal flash. Unused bytes are filled
 *  with zeros.
 *
 *   \param addr      start address in the flash memory
 *   \param data      data to be written
 *   \param data_len  number of data bytes to be written
 *   \param max_len   size of the variable in flash memory
 */
/***********************************************************************/
int32_t
Configuration::WriteString(const uint8_t *addr, const uint8_t *data, int data_len, int max_len) {
    uint8_t str[32];
    int cnt;

    LOGD("%s addr(%x) len %d\n", __FUNCTION__, GET_UINT32_ADDRESS(addr), data_len);
    //assert(max_len > sizeof(str));

    // limit number of bytes to be written to flash
    cnt = min(data_len, max_len);

    // copy string
    memcpy(str, data, cnt);

    // fill remaining space with zeros
    for (int i = cnt; i < max_len; i++) {
        str[i] = 0;
    }

    // store in flash
    return WriteFlash(GET_UINT32_ADDRESS(addr), str, max_len);
}


/***********************************************************************/
/*!
 *  Reads the string from the internal flash.
 *
 *   \param addr      start address in the flash memory
 *   \param data      read data
 *   \param data_len  number of read data bytes
 *   \param max_len   size of the variable in flash memory
 */
/***********************************************************************/
int32_t
Configuration::ReadString(const uint8_t *addr, uint8_t *data, uint8_t &data_len, int max_len) {
    data_len = 0;
    uint32_t rd_addr = GET_UINT32_ADDRESS(addr);
    uint8_t rd_buf;

    LOGD("%s addr(%x)", __FUNCTION__, GET_UINT32_ADDRESS(addr));

    for (int i = 0; i < max_len; i++) {
        if (m_otp.ReadByte(rd_addr + i, rd_buf) < 0) {
            LOGD("%s err io", __FUNCTION__);
            return -FM_HARDWARE_ERR;
        }
        if (rd_buf == 0) {
            LOGD("%s addr(%x) idx(%d) val(%x)", __FUNCTION__, rd_addr, i, rd_buf);
            return -FM_HARDWARE_ERR;
        }
        data[i] = rd_buf;
        data_len++;
    }

    return data_len;
}


/***********************************************************************/
/*!
 *   Sets the status variable in the flash memory to STATUS_SET.
 * 
 *   \param addr  address of the status
 */
/***********************************************************************/
int32_t Configuration::WriteStatus(const Status *addr) {
    Status status = STATUS_SET;

    LOGD("%s addr(%x)", __FUNCTION__, GET_UINT32_ADDRESS(addr));

    return WriteFlash(GET_UINT32_ADDRESS(addr),
                      reinterpret_cast<uint8_t *>(&status),
                      sizeof(status));
}


/***********************************************************************/
/*!
 *   Switch between user and factory mode
 * 
 *   \param enable user mode (true), factory mode (false)
 */
/***********************************************************************/
void Configuration::EnableUserMode(bool enable) {
    m_user_mode = enable;
}


/***********************************************************************/
/*!
 *  Sets the fiscal code.
 *
 *  \param data      fiscal code
 *  \param data_len  length of the fiscal code
 */
/***********************************************************************/
int32_t Configuration::SetFiscalCode(const uint8_t *data, int data_len) {
    LOGD("%s %p %s %d\n", __FUNCTION__, FM_DATA_BASE_ADDRESS->m_fiscal_code,
         data, data_len);
    int32_t ret = WriteString(FM_DATA_BASE_ADDRESS->m_fiscal_code, data, data_len, FISCAL_CODE_SIZE);
    LOGD("%s %d (%d)", __FUNCTION__, __LINE__, ret);
    if(ret > 0)
    {
        ret = WriteStatus(&(FM_DATA_BASE_ADDRESS->m_status_fiscal_code));
    }
    LOGD("%s %d (%d)", __FUNCTION__, __LINE__, ret);
    return ret;
}


/***********************************************************************/
/*!
 *  Gets the fiscal code.
 *
 *  \param data      fiscal code
 *  \param data_len  length of the fiscal code
 */
/***********************************************************************/
int32_t Configuration::GetFiscalCode(uint8_t *data, uint8_t &data_len) {
    return ReadString(FM_DATA_BASE_ADDRESS->m_fiscal_code, data, data_len, FISCAL_CODE_SIZE);
}


/***********************************************************************/
/*!
 *  Gets the fiscal code status (set/not set)
 *
 *  \return fiscal code status
 */
/***********************************************************************/
bool Configuration::GetFiscalCodeStatus() {
    if (m_data == nullptr) {
        LOGE("Flash Not Init");
        return false;
    }

    if (m_data->m_status_fiscal_code == STATUS_NOT_SET) {
        return false;
    }

    return true;
}

/***********************************************************************/
/*!
 *  Sets the fiscal number.
 *
 *  \param data     fiscal number
 *  \param data_len length of the fiscal number
 */
/***********************************************************************/
int32_t Configuration::SetFiscalNumber(const uint8_t *data, int data_len) {
    LOGD("%s %s %d", __FUNCTION__, data, data_len);
    int32_t ret = WriteString(FM_DATA_BASE_ADDRESS->m_fiscal_number, data, data_len, FISCAL_NUMBER_SIZE);
    LOGD("%s %d (%d)", __FUNCTION__, __LINE__, ret);
    if(ret > 0)
    {
        ret = WriteStatus(&(FM_DATA_BASE_ADDRESS->m_status_fiscal_number));
    }
    LOGD("%s (%d)", __FUNCTION__, ret);
    return ret;
}


/***********************************************************************/
/*!
 *  Gets the fiscal number.
 *
 *  \param data     fiscal number
 *  \param data_len length of the fiscal number
 */
/***********************************************************************/
int32_t Configuration::GetFiscalNumber(uint8_t *data, uint8_t &data_len) {
    return ReadString(FM_DATA_BASE_ADDRESS->m_fiscal_number, data, data_len, FISCAL_NUMBER_SIZE);
}


/***********************************************************************/
/*!
 *  Gets the fiscal number status (set/not set)
 *
 *  \return Fiscal code status
 */
/***********************************************************************/
bool Configuration::GetFiscalNumberStatus() {
    if (m_data == nullptr) {
        LOGE("Flash Not Init");
        return false;
    }

    if (m_data->m_status_fiscal_number == STATUS_NOT_SET)
        return false;

    return true;
}


/***********************************************************************/
/*!
 *  Sets the fiscal revolving amount
 *
 *  \param data      fiscal revolving amount
 *  \param data_len  length of the fiscal revolving amount
 */
/***********************************************************************/
void Configuration::SetFiscalRevolvingAmount(const uint8_t *data, int data_len) {
    int cnt;
    uint8_t tmp[Z_REPORT_ENTRY_SIZE];

    // limit entry size
    cnt = min(data_len, Z_REPORT_ENTRY_SIZE);

    // copy data
    for (int i = 0; i < cnt; i++)
        tmp[i] = data[i];

    // fill used bytes with default value
    for (int i = cnt; i < Z_REPORT_ENTRY_SIZE; i++)
        tmp[i] = 0xff;

    // write data to flash
    if (m_user_mode == false) {
        WriteFlash(
                GET_UINT32_ADDRESS((FM_DATA_BASE_ADDRESS->m_factory_mode_data.m_revolving_amount)),
                tmp,
                Z_REPORT_ENTRY_SIZE);
        WriteStatus(&(FM_DATA_BASE_ADDRESS->m_status_factory_mode_revolving_amount));
    } else {
        WriteFlash(GET_UINT32_ADDRESS((FM_DATA_BASE_ADDRESS->m_user_mode_data.m_revolving_amount)),
                   tmp,
                   Z_REPORT_ENTRY_SIZE);
        WriteStatus(&(FM_DATA_BASE_ADDRESS->m_status_user_mode_revolving_amount));
    }

    // store new status in flash
    if (m_user_mode == false)
        WriteStatus(&(FM_DATA_BASE_ADDRESS->m_status_factory_mode_revolving_amount));
    else
        WriteStatus(&(FM_DATA_BASE_ADDRESS->m_status_user_mode_revolving_amount));
}


/***********************************************************************/
/*!
 *  Gets the fiscal revolving amount
 *
 *  \param data      fiscal revolving amount
 *  \param data_len  length of the fiscal revolving amount
 */
/***********************************************************************/
void Configuration::GetFiscalRevolvingAmount(uint8_t *data, uint8_t &data_len) {
    int cnt;

    if (m_data == nullptr) {
        LOGE("Flash Not Init");
        return;
    }

    cnt = min((int) data_len, Z_REPORT_ENTRY_SIZE);

    if (m_user_mode == false)
        memcpy(data, m_data->m_factory_mode_data.m_revolving_amount, cnt);
    else
        memcpy(data, m_data->m_user_mode_data.m_revolving_amount, cnt);

    data_len = cnt;
}


/***********************************************************************/
/*!
 *  Get fiscal revolving amount status
 *
 *   \return fiscal revolving amount status
 */
/***********************************************************************/
bool Configuration::GetFiscalRevolvingAmountStatus() {
    Status status;

    if (m_data == nullptr) {
        LOGE("Flash Not Init");
        return false;
    }

    if (m_user_mode == false)
        status = m_data->m_status_factory_mode_revolving_amount;
    else
        status = m_data->m_status_user_mode_revolving_amount;

    if (status == STATUS_NOT_SET)
        return false;

    return true;
}


/***********************************************************************/
/*!
 *  Calculates the values returned by GetNumberOfEntries().
 */
/***********************************************************************/
void Configuration::CalculateNumberOfEntries() {
    bool used;

    if (m_data == nullptr) {
        LOGE("Flash Not Init");
        return;
    }
    m_number_of_entries_factory_mode = 0;
    m_number_of_entries_user_mode = 0;

    // search first unused entry (factory mode)
    for (int i = 0; i < NUMBER_OF_Z_REPORTS_FACTORY_MODE; i++) {
        used = false;

        ZReportEntry const &entry = m_data->m_factory_mode_data.m_z_reports[i];

        for (int a = 0; a < Z_REPORT_ENTRY_SIZE; a++) {
            if (entry[a] != 0xff) {
                used = true;
                break;
            }
        }

        if (used == false) {
            m_number_of_entries_factory_mode = i;
            break;
        }
    }

    // all entries used (factory mode)?
    if (used == true)
        m_number_of_entries_factory_mode = NUMBER_OF_Z_REPORTS_FACTORY_MODE;

    // search first unused entry (user mode)
    for (int i = 0; i < NUMBER_OF_Z_REPORTS_USER_MODE; i++) {
        used = false;

        ZReportEntry const &entry = m_data->m_user_mode_data.m_z_reports[i];

        for (int a = 0; a < Z_REPORT_ENTRY_SIZE; a++) {
            if (entry[a] != 0xff) {
                used = true;
                break;
            }
        }

        if (used == false) {
            m_number_of_entries_user_mode = i;
            break;
        }
    }

    // all entries used (factory mode)?
    if (used == true)
        m_number_of_entries_user_mode = NUMBER_OF_Z_REPORTS_USER_MODE;
}


/***********************************************************************/
/*!
 *  Gets number of used entries.
 *
 *   \param number of used entries
 */
/***********************************************************************/

uint32_t Configuration::GetNumberOfEntries() {
    if (m_user_mode == false)
        return m_number_of_entries_factory_mode;
    else
        return m_number_of_entries_user_mode;
}


/***********************************************************************/
/*!
 *  Set fiscal memory entry.
 *
 *  \param data      fiscal memory entry
 *  \param data_len  length of the fiscal memory entry
 */
/***********************************************************************/
void Configuration::SetEntry(const uint8_t *data, int data_len) {
    int index;
    int cnt;
    uint8_t tmp[Z_REPORT_ENTRY_SIZE];

    if (m_data == nullptr) {
        LOGE("Flash Not Init");
        return;
    }

    // get index of next free item
    index = GetNumberOfEntries();

    // limit entry size
    cnt = min(data_len, Z_REPORT_ENTRY_SIZE);

    // copy data
    for (int i = 0; i < cnt; i++)
        tmp[i] = data[i];

    // fill unused bytes with "0xff"
    for (int i = cnt; i < Z_REPORT_ENTRY_SIZE; i++)
        tmp[i] = 0xff;

    // write entry to flash and update value returned by GetNumberOfEntries()
    if (m_user_mode == false) {
        if (index < NUMBER_OF_Z_REPORTS_FACTORY_MODE) {
            WriteFlash(GET_UINT32_ADDRESS(m_data->m_factory_mode_data.m_z_reports[index]),
                       tmp,
                       Z_REPORT_ENTRY_SIZE);
            m_number_of_entries_factory_mode++;
        }
    } else {
        if (index < NUMBER_OF_Z_REPORTS_USER_MODE) {
            WriteFlash(GET_UINT32_ADDRESS(m_data->m_user_mode_data.m_z_reports[index]),
                       tmp,
                       Z_REPORT_ENTRY_SIZE);
            m_number_of_entries_user_mode++;
        }
    }
}


/***********************************************************************/
/*!
 *  Get fiscal memory entry.
 *
 *  \param index     index of the entry
 *  \param data      fiscal memory entry
 *  \param data_len  length of the fiscal memory entry
 */
/***********************************************************************/
void Configuration::GetEntry(int index, uint8_t *data, uint8_t &data_len) {
    int cnt;

    if (m_data == nullptr) {
        LOGE("Flash Not Init");
        return;
    }

    cnt = min((int) data_len, Z_REPORT_ENTRY_SIZE);

    if (m_user_mode == false)
        memcpy(data, m_data->m_factory_mode_data.m_z_reports[index], cnt);
    else
        memcpy(data, m_data->m_user_mode_data.m_z_reports[index], cnt);

    data_len = cnt;
}

/***********************************************************************/
/*!
 *  Get the "daily sales total" of the fiscal memory entry.
 *
 *  \param index     index of the entry
 *  \return          daily sales total
 */
/***********************************************************************/
uint32_t Configuration::GetDailySalesTotal(int index) {
    const uint8_t *data;
    uint32_t result;

    if (m_data == nullptr) {
        LOGE("Flash Not Init");
        return 0;
    }

    if (m_user_mode == false)
        data = m_data->m_factory_mode_data.m_z_reports[index];
    else
        data = m_data->m_user_mode_data.m_z_reports[index];

    result = data[0] |
             (data[1] << 8) |
             (data[2] << 16) |
             (data[3] << 24);

    return result;
}

/***********************************************************************/
/*!
 *  Get the "daily sales tax total" of the fiscal memory entry.
 *
 *  \param index     index of the entry
 *  \return          daily sales tax total
 */
/***********************************************************************/
uint32_t Configuration::GetDailySalesTaxTotal(int index) {
    const uint8_t *data;
    uint32_t result;

    if (m_data == nullptr) {
        LOGE("Flash Not Init");
        return 0;
    }

    if (m_user_mode == false)
        data = m_data->m_factory_mode_data.m_z_reports[index];
    else
        data = m_data->m_user_mode_data.m_z_reports[index];

    result = data[4] |
             (data[5] << 8) |
             (data[6] << 16) |
             (data[7] << 24);

    return result;
}


/***********************************************************************/
/*!
 *  Get the "date/time" of the fiscal memory entry.
 *
 *  \param index     index of the entry
 *  \return          date/time [seconds]
 */
/***********************************************************************/
uint32_t Configuration::GetDateTime(int index) {
    const uint8_t *data;
    uint32_t year;
    uint32_t month;
    uint32_t day;
    uint32_t hour;
    uint32_t minute;

    if(m_data == nullptr)
    {
        LOGE("Flash Not Init");
        return 0;
    }
    // get entry
    if (m_user_mode == false)
        data = m_data->m_factory_mode_data.m_z_reports[index];
    else
        data = m_data->m_user_mode_data.m_z_reports[index];

    // get "year"
    year = data[8];

    // get "month"
    month = data[9] & 0x0f;

    // get "day"
    day = (data[9] >> 4) | ((data[10] & 0x01) << 4);

    // get "hour"
    hour = (data[10] >> 1) & 0x1f;

    // get "minute"
    minute = ((data[10] >> 6) & 0x03) | ((data[11] & 0x0f) << 2);

    // calculate/return result
    return minute + hour * 60 + day * 24 * 60 + month * 24 * 60 * 31 + year * 24 * 60 * 31 * 12;
}


/***********************************************************************/
/*!
 *  Get number of free fiscal memory entries.
 *
 *   \return number of free fiscal memory entries
 */
/***********************************************************************/
uint32_t Configuration::GetEntrySpace() {
    uint32_t count;

    if(m_data == nullptr)
    {
        LOGE("Flash Not Init");
        return 0;
    }
    count = GetNumberOfEntries();

    if (m_user_mode == false)
        return NUMBER_OF_Z_REPORTS_FACTORY_MODE - count;
    else
        return NUMBER_OF_Z_REPORTS_USER_MODE - count;
}


/***********************************************************************/
/*!
 *  Get Fiscal memory write status 
 *
 *   \return full (true), not full (false)
 */
/***********************************************************************/
bool Configuration::GetFullStatus() {
    Status status;

    if(m_data == nullptr)
    {
        LOGE("Flash Not Init");
        return false;
    }

    if (m_user_mode == false)
        status = m_data->m_status_factory_mode_full;
    else
        status = m_data->m_status_user_mode_full;

    if (status == STATUS_NOT_SET)
        return false;

    return true;
}


/***********************************************************************/
/*!
 *  Mark fiscal memory as full
 */
/***********************************************************************/
void Configuration::SetFullStatus() {
    if (m_user_mode == false)
        WriteStatus(&(m_data->m_status_factory_mode_full));
    else
        WriteStatus(&(m_data->m_status_user_mode_full));
}

str_fmInfo Configuration::GetFmInfo()
{
    return m_otp.GetFmInfo();
}