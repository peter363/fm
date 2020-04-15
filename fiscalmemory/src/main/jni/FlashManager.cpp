/*************************************************************

   Copyright (c) 2008 telos EDV Systementwicklung GmbH,
   Hamburg (Germany)

                    http://www.telos.de

   $Id: FlashManager.cpp 3827 2010-10-04 11:47:54Z mbudde $

   This file is part of "FM080525A - Fiscal Cash Register".

 *************************************************************/
#define TAG "FlashManager"

#include "FlashManager.h"
#include "dbg_log.h"
// #include "SmbCommands.h"


#include <cassert>

#ifdef DEBUG_FLASH_MANAGER
  #include <cstdio>
#endif

//using namespace std;
// using namespace Farimex;

/************************************************************************/
/*!
 *  Constructor
 */
/************************************************************************/
FlashManager::FlashManager() : m_config()
{
  LOGD("%s", __FUNCTION__);
  m_entry_number = 0;
  m_get_daily_sales_total_start_idx = -1;
  m_get_daily_sales_total_stop_idx = -1;
  m_hardware_fault = false;
}

FlashManager::~FlashManager()
{
  LOGD("%s", __FUNCTION__);
}

int32_t FlashManager::Init(FM_CONTEXT_t const *fm_ctx)
{
    return m_config.Init(fm_ctx);
}

bool FlashManager::IsInited()
{
    return m_config.IsInited();
}

/***********************************************************************/
/*!
 *  Set Fiscal code
 *
 *  \param pBuffer Pointer to data buffer
 *  \param number_of_bytes number of bytes to store
 *
 *  \return error code
 */
/***********************************************************************/
int FlashManager::SetFiscalCode(const uint8_t* pBuffer, int number_of_bytes)
{
  // assertions
  if(pBuffer == nullptr)
  {
    LOGE("%s %d CMD_ARGUMENT_INVALID", __FUNCTION__, __LINE__);
    return CMD_ARGUMENT_INVALID;
  }
//  assert(number_of_bytes == FISCAL_CODE_MAX_SIZE);

  // check parameters
  if((number_of_bytes < FISCAL_CODE_MIN_SIZE) ||
     (number_of_bytes > FISCAL_CODE_MAX_SIZE))
  {
      LOGE("CMD_DATA_INCOMPLETE");
      return CMD_DATA_INCOMPLETE;
  }

  // already set?
  if (m_config.GetFiscalCodeStatus() == true)
  {
      LOGE("CMD_FISCAL_NUMBER_AND_CODE_HAS_BEEN_ALREADY_SET");
      return CMD_FISCAL_NUMBER_AND_CODE_HAS_BEEN_ALREADY_SET;
  }

  // set fiscal code
  if(m_config.SetFiscalCode(pBuffer, number_of_bytes) != FM_SUCCESS)
  {
	  LOGE("%s %d CMD_HARDWARE_FAULT", __FUNCTION__, __LINE__);
      return CMD_HARDWARE_FAULT;
  }
  return CMD_OK;
}

/***********************************************************************/
/*!
 *  Get fiscal code
 *
 *  \param pBuffer Pointer to data buffer
 *  \param number_of_bytes number of bytes to store
 *
 *  \return error code
 */
/***********************************************************************/
int FlashManager::GetFiscalCode(uint8_t* pBuffer, uint8_t &number_of_bytes)
{
  // assertions
  if(pBuffer == nullptr)
  {
    LOGE("%s %d CMD_ARGUMENT_INVALID", __FUNCTION__, __LINE__);
    return CMD_ARGUMENT_INVALID;
  }
//  assert(number_of_bytes == FISCAL_CODE_MAX_SIZE);

  // check parameters
  if(number_of_bytes != FISCAL_CODE_MAX_SIZE)
  {
      LOGE("%s CMD_DATA_INCOMPLETE", __FUNCTION__);
      return CMD_DATA_INCOMPLETE;
  }

  LOGD("%s m_config.GetFiscalCodeStatus", __FUNCTION__);
  // fiscal code set?
  if (m_config.GetFiscalCodeStatus() == false)
  {
      LOGE("%s CMD_FISCAL_NUMBER_AND_CODE_NOT_SET", __FUNCTION__);
      return CMD_FISCAL_NUMBER_AND_CODE_NOT_SET;
  }

  LOGD("%s m_config.GetFiscalCode", __FUNCTION__);
  // get fiscal code
  if(m_config.GetFiscalCode(pBuffer, number_of_bytes) <= 0)
  {
     LOGE("%s %d CMD_HARDWARE_FAULT", __FUNCTION__, __LINE__);
     return CMD_HARDWARE_FAULT;
  }
  return CMD_OK;
}

/***********************************************************************/
/*!
 *  Set Fiscal number
 *
 *  \param pBuffer Pointer to data buffer
 *  \param number_of_bytes number of bytes to store
 *
 *  \return error code
 */
/***********************************************************************/
int FlashManager::SetFiscalNumber(const uint8_t* pBuffer, int number_of_bytes)
{
  // assertions
  if(pBuffer == nullptr)
  {
    LOGE("%s %d CMD_ARGUMENT_INVALID", __FUNCTION__, __LINE__);
    return CMD_ARGUMENT_INVALID;
  }

  // check parameters
  if(number_of_bytes != FISCAL_NUMBER_MAX_SIZE) {
    LOGE("%s %d CMD_DATA_INCOMPLETE", __FUNCTION__, __LINE__);
    return CMD_DATA_INCOMPLETE;
  }

  // check paramezers
  if((number_of_bytes < FISCAL_NUMBER_MIN_SIZE) ||
     (number_of_bytes > FISCAL_NUMBER_MAX_SIZE))
  {
	  LOGE("%s %d CMD_DATA_INCOMPLETE", __FUNCTION__, __LINE__);
      return CMD_DATA_INCOMPLETE;
  }

  // already set?
  if (m_config.GetFiscalNumberStatus() == true)
  {
	  LOGE("%s %d CMD_FISCAL_NUMBER_AND_CODE_HAS_BEEN_ALREADY_SET", __FUNCTION__, __LINE__);
      return CMD_FISCAL_NUMBER_AND_CODE_HAS_BEEN_ALREADY_SET;
  }

  // set fiscal number
  if(m_config.SetFiscalNumber(pBuffer, number_of_bytes) != FM_SUCCESS)
  {
	  LOGE("%s %d CMD_HARDWARE_FAULT", __FUNCTION__, __LINE__);
      return CMD_HARDWARE_FAULT;
  }

  return CMD_OK;

}

/***********************************************************************/
/*!
 *  Get fiscal number
 *
 *  \param pBuffer Pointer to data buffer
 *  \param number_of_bytes number of bytes to store
 *  \return error code
 */
/***********************************************************************/
int FlashManager::GetFiscalNumber(uint8_t* pBuffer, uint8_t &number_of_bytes)
{
  // assertions
  if(pBuffer == nullptr)
  {
    LOGE("%s %d CMD_ARGUMENT_INVALID", __FUNCTION__, __LINE__);
    return CMD_ARGUMENT_INVALID;
  }

  // check parameters
  if(number_of_bytes != FISCAL_NUMBER_MAX_SIZE) {
    LOGE("%s %d CMD_DATA_INCOMPLETE", __FUNCTION__, __LINE__);
    return CMD_DATA_INCOMPLETE;
  }

  // fiscal number set?
  if (m_config.GetFiscalNumberStatus() == false)
  {
	  LOGE("%s %d CMD_FISCAL_NUMBER_AND_CODE_NOT_SET", __FUNCTION__, __LINE__);
      return CMD_FISCAL_NUMBER_AND_CODE_NOT_SET;
  }

  // get fiscal number
  if(m_config.GetFiscalNumber(pBuffer, number_of_bytes) <= 0)
  {
	  LOGE("%s %d CMD_HARDWARE_FAULT", __FUNCTION__, __LINE__);
      return CMD_HARDWARE_FAULT;
  }

  return CMD_OK;
}

/***********************************************************************/
/*!
 *  Store Entry data
 *
 *  \param pBuffer Pointer to data buffer
 *  \param number_of_bytes number of bytes to store
 *  \return error code
 */
/***********************************************************************/
int FlashManager::SetEntry(const uint8_t* pBuffer, int number_of_bytes)
{
  // assertions
  if(pBuffer == nullptr)
  {
    LOGE("%s %d CMD_ARGUMENT_INVALID", __FUNCTION__, __LINE__);
  	return CMD_ARGUMENT_INVALID;
  }

  // check parameters
  if(number_of_bytes != ENTRY_SIZE)
  {
    LOGE("%s %d CMD_DATA_INCOMPLETE", __FUNCTION__, __LINE__);
    return CMD_DATA_INCOMPLETE;
  }

  // module full?
  if(m_config.GetFullStatus() == true)
  {
    LOGE("%s %d CMD_NO_SPACE_LEFT", __FUNCTION__, __LINE__);
    return CMD_NO_SPACE_LEFT;
  }

  // store entry
  m_config.SetEntry(pBuffer, number_of_bytes);

  // mark module as full?
  if (m_config.GetEntrySpace() == 0)
  {
    m_config.SetFullStatus();
  }

  return CMD_OK;
}

/***********************************************************************/
/*!
 *  Get number of stored entries
 *
 *  \return Number of stored entries
 */
/***********************************************************************/
uint32_t FlashManager::GetNumberOfEntries()
{
  return m_config.GetNumberOfEntries();
}

/***********************************************************************/
/*!
 *  Gets the current status of the external memory (OTP)
 *
 *  \return Error code of external flash
 */
/***********************************************************************/
int FlashManager::GetFlashStatus()
{
  if(m_hardware_fault == true)
  {
    LOGE("%s %d CMD_HARDWARE_FAULT", __FUNCTION__, __LINE__);
    return CMD_HARDWARE_FAULT;
  }

  if(m_config.GetFullStatus() == true)
  {
    LOGE("%s %d CMD_NO_SPACE_LEFT", __FUNCTION__, __LINE__);
    return CMD_NO_SPACE_LEFT;
  }

  return CMD_OK;
}

/***********************************************************************/
/*!
 *  Set entry number next to read.
 *
 *  \param Entry number
 *  \return Error code
 */
/***********************************************************************/
int FlashManager::SetEntryNumber(uint32_t entry_number)
{
  if(entry_number >= GetNumberOfEntries())
  {
    LOGE("%s %d CMD_ARGUMENT_INVALID", __FUNCTION__, __LINE__);
    return CMD_ARGUMENT_INVALID;
  }

  m_entry_number = entry_number;

  return CMD_OK;
}

/***********************************************************************/
/*!
 *  Get Entry data
 *
 *  \param pBuffer Pointer to data Buffer
 *  \param number_of_bytes number of bytes to store
 */
/***********************************************************************/
int FlashManager::GetEntry(uint8_t* pBuffer, uint8_t &number_of_bytes)
{
  // assertions
  if(pBuffer == nullptr)
  {
    LOGE("%s %d CMD_ARGUMENT_INVALID", __FUNCTION__, __LINE__);
    return CMD_ARGUMENT_INVALID;
  }
  if(number_of_bytes != ENTRY_SIZE) {
    LOGE("%s %d CMD_DATA_INCOMPLETE", __FUNCTION__, __LINE__);
    return CMD_DATA_INCOMPLETE;
  }
  // check parameters
  if(number_of_bytes != ENTRY_SIZE)
  {
    LOGE("%s %d CMD_ARGUMENT_INVALID", __FUNCTION__, __LINE__);
    return CMD_ARGUMENT_INVALID;
  }

  // check index
  if(m_entry_number >= GetNumberOfEntries())
  {
    LOGE("%s %d CMD_ARGUMENT_INVALID", __FUNCTION__, __LINE__);
    return CMD_ARGUMENT_INVALID;
  }

  // get entry
  m_config.GetEntry(m_entry_number, pBuffer, number_of_bytes);

  return CMD_OK;
}

/***********************************************************************/
/*!
 *  Get the sum of all "daily sales total" values.
 *
 *  \return  sum of daily sales total
 */
/***********************************************************************/
uint64_t FlashManager::GetDailySalesTotal()
{
  uint64_t  result;
  uint32_t  entries;


  result = 0;
  entries = GetNumberOfEntries();

  for (int i=0; i < entries; i++)
    result += m_config.GetDailySalesTotal (i);

  return result;
}

/***********************************************************************/
/*!
 *  Get the sum of all "daily sales tax total" values.
 *
 *  \return  sum of daily sales tax total
 */
/***********************************************************************/
uint64_t FlashManager::GetDailySalesTaxTotal()
{
  uint64_t  result;
  uint32_t  entries;


  result = 0;
  entries = GetNumberOfEntries();

  for (int i=0; i < entries; i++)
    result += m_config.GetDailySalesTaxTotal (i);

  return result;
}

/***********************************************************************/
/*!
 *  Sets the start and stop index for GetDailySalesTotal_Range()
 *  and GetDailySalesTaxTotal_Range().
 *
 *   \param start_idx  start index
 *   \param stop_idx   stop index
 *   \return           error code
 */
/***********************************************************************/
int FlashManager::SetDailySalesTotalStartStop_Index (uint32_t start_idx, uint32_t stop_idx)
{
  if ((start_idx > stop_idx) ||
      (stop_idx >= GetNumberOfEntries()))
    return CMD_ARGUMENT_INVALID;

  m_get_daily_sales_total_start_idx = start_idx;
  m_get_daily_sales_total_stop_idx = stop_idx;

  return CMD_OK;
}

/***********************************************************************/
/*!
 *  Sets the start and stop date/time for GetDailySalesTotal_Range()
 *  and GetDailySalesTaxTotal_Range().
 *
 *   \param start_date_time  start date/time
 *   \param stop_date_time   stop date/time
 *   \return                 error code
 */
/***********************************************************************/
int FlashManager::SetDailySalesTotalStartStop_DateTime (uint32_t start_date_time, uint32_t stop_date_time)
{
  uint32_t  entries;


  // check parameters
  if (start_date_time > stop_date_time)
    return CMD_ARGUMENT_INVALID;

  // set default range (no entry)
  m_get_daily_sales_total_start_idx = -1;
  m_get_daily_sales_total_stop_idx = -1;

  // get number of entries
  entries = GetNumberOfEntries();

  // search start index
  for (int i=0; i < entries; i++)
  {
    if (m_config.GetDateTime (i) >= start_date_time)
    {
      if (m_config.GetDateTime (i) <= stop_date_time)
        m_get_daily_sales_total_start_idx = i;
      break;
    }
  }

  // no entry within the specified range?
  if (m_get_daily_sales_total_start_idx == -1)
    return CMD_OK;

  // search stop index
  for (int i=m_get_daily_sales_total_start_idx; i < entries; i++)
  {
    if (m_config.GetDateTime (i) <= stop_date_time)
      m_get_daily_sales_total_stop_idx = i;
    else
      break;
  }

  return CMD_OK;
}

/***********************************************************************/
/*!
 *  Get the sum of all "daily sales total" values within the
 *  range specified by SetDailySalesTotalStartStop_Index() or
 *  SetDailySalesTotalStartStop_DateTime().
 *
 *  \return  sum of daily sales total
 */
/***********************************************************************/
uint64_t FlashManager::GetDailySalesTotal_Range()
{
  uint64_t  result;


  result = 0;

  if ((m_get_daily_sales_total_start_idx == -1) ||
      (m_get_daily_sales_total_stop_idx == -1))
    return result;

  for (int i=m_get_daily_sales_total_start_idx; i <= m_get_daily_sales_total_stop_idx; i++)
    result += m_config.GetDailySalesTotal (i);

  return result;
}

/***********************************************************************/
/*!
 *  Get the sum of all "daily sales tax total" values within the
 *  range specified by SetDailySalesTotalStartStop_Index() or
 *  SetDailySalesTotalStartStop_DateTime().
 *
 *  \return  sum of daily sales tax total
 */
/***********************************************************************/
uint64_t FlashManager::GetDailySalesTaxTotal_Range()
{
  uint64_t  result;


  result = 0;

   if ((m_get_daily_sales_total_start_idx == -1) ||
      (m_get_daily_sales_total_stop_idx == -1))
    return result;

  for (int i=m_get_daily_sales_total_start_idx; i <= m_get_daily_sales_total_stop_idx; i++)
    result += m_config.GetDailySalesTaxTotal (i);

  return result;
}

/***********************************************************************/
/*!
 *  Set fiscal revolving amount
 *
 *  \param pBuffer Pointer to data Buffer
 *  \param number_of_bytes number of bytes to store
 *  \return Error code
 */
/***********************************************************************/
int FlashManager::SetFiscalRevolvingAmount(const uint8_t* pBuffer, int number_of_bytes)
{
  // assertions
  if(pBuffer == nullptr)
  {
      return CMD_ARGUMENT_INVALID;
  }

  // check parameters
  if(number_of_bytes != REVOLVING_AMOUNT_SIZE)
  {
      return CMD_ARGUMENT_INVALID;
  }

  // already set?
  if (m_config.GetFiscalRevolvingAmountStatus() == true)
    return CMD_FISCAL_NUMBER_AND_CODE_HAS_BEEN_ALREADY_SET;

  // store fiscal revolving amount
  m_config.SetFiscalRevolvingAmount(pBuffer, number_of_bytes);

  return CMD_OK;
}

/***********************************************************************/
/*!
 *  Get fiscal revolving amount
 *
 *  \param pBuffer Pointer to data Buffer
 *  \param number_of_bytes number of bytes to store
 *  \return true if entry finished and false if entry data not finished
 */
/***********************************************************************/
int FlashManager::GetFiscalRevolvingAmount(uint8_t* pBuffer, uint8_t &number_of_bytes)
{
  // assertions
  if(pBuffer == nullptr)
      return CMD_ARGUMENT_INVALID;

  // check parameters
  if(number_of_bytes != REVOLVING_AMOUNT_SIZE)
    return CMD_ARGUMENT_INVALID;

  // revolving amount set?
  if (m_config.GetFiscalRevolvingAmountStatus() == false)
    return CMD_FISCAL_REVOLVING_AMOUNT_NOT_SET;

  // get revolving amount
  m_config.GetFiscalRevolvingAmount(pBuffer, number_of_bytes);

  return CMD_OK;
}

/***********************************************************************/
/*!
 *  Set Mode (switch between user and factory mode)
 *
 *  \param error code
 */
/***********************************************************************/
int FlashManager::SetMode(uint8_t mode)
{
  switch(mode)
  {
    case 1:
      m_config.EnableUserMode(false);
      break;
    case 0:
      m_config.EnableUserMode(true);
      break;
    default:
      return CMD_ARGUMENT_INVALID;
  }

  return CMD_OK;
}

/***********************************************************************/
/*!
 *  Get fiscal memory status
 *
 *  \return true if numbers codes was set.
 */
/***********************************************************************/
bool FlashManager::GetFullStatus()
{
  return m_config.GetFullStatus();
}

/***********************************************************************/
/*!
 *  Gets the free memory space of external flash (OTP)
 *
 *  \return free entries
 */
/***********************************************************************/
uint32_t FlashManager::GetEntrySpace()
{
  return m_config.GetEntrySpace();
}

/***********************************************************************/
/*!
 *  Clear complete fiscal memory card
 */
/***********************************************************************/
void FlashManager::ClearCompleteCard()
{
  m_config.ClearFlash();

  m_entry_number = 0;
  m_get_daily_sales_total_start_idx = -1;
  m_get_daily_sales_total_stop_idx = -1;
}

str_fmInfo FlashManager::GetFmInfo()
{
    str_fmInfo info = { 0 };
    if(IsInited())
    {
      info = m_config.GetFmInfo();
    }

    return info;
}
