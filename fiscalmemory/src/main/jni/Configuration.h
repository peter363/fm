
/*************************************************************

   Copyright (c) 2008 telos EDV Systementwicklung GmbH,
   Hamburg (Germany)

                    http://www.telos.de

   $Id: Configuration.h 3816 2010-10-01 08:37:13Z mbudde $

   This file is part of "FM080525A - Fiscal Cash Register".

 *************************************************************/


#ifndef FARIMEX_CONFIGURATION
#define FARIMEX_CONFIGURATION

#include <stdint.h>
#include "FmContext.h"
#include "OtpMonitor.h"

/***********************************************************************/
/*!
  *   Class to store the data of the fiscal memory in the internal
  *   flash memory of the STM32.
  */
/***********************************************************************/
class Configuration
{
  public:
    //! number of bytes used for the "fiscal number" (multiple of "4")
    static const int FISCAL_NUMBER_SIZE = 16;

    //! number of bytes used for the "fiscal code" (multiple of "4")
    static const int FISCAL_CODE_SIZE = 16;

    //! number of bytes used for one "Z report entry" (multiple of "4")
    static const int Z_REPORT_ENTRY_SIZE = 16;

    //! number of "Z report entries" in "factory mode"
    static const int NUMBER_OF_Z_REPORTS_FACTORY_MODE = 12;

    //! number of "Z report entries" in "user mode"
    static const int NUMBER_OF_Z_REPORTS_USER_MODE = 2500;

  private:
    //! size of one flash page
    static const int  FLASH_PAGE_SIZE = 1024;

    //! constant used to signal that the memory has been cleared
    static const uint32_t CLEARED = 0x12345678;

  private:
    //! type to store the data of one "Z report"
    typedef uint8_t ZReportEntry[Z_REPORT_ENTRY_SIZE];


    //! type to store a status
    enum Status
    {
      STATUS_SET     = 0x00000000,    //!< set
      STATUS_NOT_SET = 0xffffffff     //!< not set
    };


    //! type to store the data of the factory mode
    struct FactoryModeData
    {
      //! fiscal revolving amount
      ZReportEntry  m_revolving_amount;

      //! Z reports
      ZReportEntry  m_z_reports[NUMBER_OF_Z_REPORTS_FACTORY_MODE];
    } __attribute__ ((__packed__));;


    //! type to store the data of the factory mode
    struct UserModeData
    {
      //! fiscal revolving amount
      ZReportEntry  m_revolving_amount;

      //! Z reports
      ZReportEntry  m_z_reports[NUMBER_OF_Z_REPORTS_USER_MODE];
    } __attribute__ ((__packed__));;


    //! type to store all data
    struct Data
    {
      //! cleared after factory?
      uint32_t  m_cleared;

      //! fiscal code
      uint8_t  m_fiscal_code[FISCAL_CODE_SIZE];

      //! fiscal number
      uint8_t  m_fiscal_number[FISCAL_NUMBER_SIZE];

      //! data of factory mode
      FactoryModeData  m_factory_mode_data;

      //! data of user mode
      UserModeData  m_user_mode_data;

      //! fiscal code set?
      Status  m_status_fiscal_code;

      //! fiscal number set?
      Status  m_status_fiscal_number;

      //! factory mode: fiscal revolving amount set?
      Status m_status_factory_mode_revolving_amount;

      //! factory mode: full?
      Status m_status_factory_mode_full;

      //! user mode: fiscal revolving amount set?
      Status m_status_user_mode_revolving_amount;

      //! user mode: full?
      Status m_status_user_mode_full;
    } __attribute__ ((__packed__));


  public:
    Configuration();
    ~Configuration();
    void ClearFlash();

    int32_t Init(FM_CONTEXT_t const *fm_ctx);
    bool IsInited();
    str_fmInfo GetFmInfo();

    bool GetFullStatus();
    void SetFullStatus();

    int32_t SetFiscalCode(const uint8_t* data, int data_len);
    int32_t GetFiscalCode(uint8_t* data, uint8_t &data_len);
    bool GetFiscalCodeStatus();

    int32_t SetFiscalNumber(const uint8_t* data, int data_len);
    int32_t GetFiscalNumber(uint8_t* data, uint8_t &data_len);
    bool GetFiscalNumberStatus();

    void SetFiscalRevolvingAmount(const uint8_t* data, int data_len);
    void GetFiscalRevolvingAmount(uint8_t* data, uint8_t &data_len);
    bool GetFiscalRevolvingAmountStatus();

    uint32_t GetNumberOfEntries();
    uint32_t GetEntrySpace();

    void SetEntry(const uint8_t* data, int data_len);
    void GetEntry(int index, uint8_t* data, uint8_t &data_len);

    uint32_t GetDailySalesTotal (int index);
    uint32_t GetDailySalesTaxTotal (int index);
    uint32_t GetDateTime (int index);

    void EnableUserMode(bool enable);

  private:
    int32_t WriteFlash (uint32_t addr, const uint8_t *data, int data_len);
    int32_t WriteString (const uint8_t *addr, const uint8_t *data, int data_len, int max_len);
    int32_t ReadString (const uint8_t *addr, uint8_t *data, uint8_t &data_len, int max_len);
    int32_t WriteStatus (const Status *addr);
    void CalculateNumberOfEntries();

  private:
    //! start address of the configuration in the flash memory
    //static const uint32_t  ADDRESS_DATA_IN_FLASH;

    //! configuration in the flash memory
    Data const *m_data;
    OtpMonitor m_otp;

  private:
    //! number of used flash pages
    int  m_flash_pages;

    //! currently selected mode (user or factory)
    bool  m_user_mode;

    uint32_t  m_number_of_entries_factory_mode;

    uint32_t  m_number_of_entries_user_mode;
};


#endif
