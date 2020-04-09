/*************************************************************
 
   Copyright (c) 2008 telos EDV Systementwicklung GmbH,
   Hamburg (Germany)

                    http://www.telos.de

   $Id: FlashManager.h 3818 2010-10-01 10:33:25Z mbudde $

   This file is part of "FM080525A - Fiscal Cash Register".

 *************************************************************/

#ifndef FLASH_MANAGER_H
#define FLASH_MANAGER_H

#include "Configuration.h"



  /***********************************************************************/
  /*!
  *   Implements the Manager to handles every Flash operation and the 
  *   de- and encryption of the entry data.
  */
  /***********************************************************************/
  
class FlashManager
{
  private:    
    // Commands
    static const uint8_t SET_RANDOM_CR = 0;                   //!< Receive random number from cash register
    static const uint8_t GET_RESPONCE_FM = 1;                 //!< Send xor rnd_cr number to cash register
    static const uint8_t GET_RANDOM_FM = 2;                   //!< Send random number to cash register
    static const uint8_t SET_RESPONCE_CR = 3;                 //!< Receive xor rnd_ej number from cash register
    
    static const uint8_t SET_FISCAL_CODE = 4;                 //!< Receive fiscal code
    static const uint8_t GET_FISCAL_CODE = 5;                 //!< Send fiscal code
    static const uint8_t SET_FISCAL_NUMBER = 6;               //!< Receive fiscal number
    static const uint8_t GET_FISCAL_NUMBER = 7;               //!< Send fiscal number

    static const uint8_t SEND_DATA_OF_ENTRY = 9;              //!< Receive entry data from cr
    static const uint8_t GET_NUMBER_OF_ENTRIES = 11;          //!< Send number of stored entries
    static const uint8_t SET_ENTRY_INDEX = 12;                //!< Receive index to a stored entry
    static const uint8_t GET_ENTRY_DATA = 13;                 //!< Send data of stored entry

    static const uint8_t GET_FREE_ENTRIES = 15;               //!< Send memory space info (used and free)

    static const uint8_t CLEAR_COMPLETE_CARD = 16;            //!< Reset complete card

    static const uint8_t SOFTWARE_RESET = 17;                 //!< Reset ej firmware (go to not authenticated mode)
    static const uint8_t SET_FISCAL_REVOLING_AMOUT = 18;      //!< Receive new code page
    static const uint8_t GET_FISCAL_REVOLING_AMOUT = 19;      //!< Send current used code page number
    static const uint8_t SET_MODE = 20;                       //!< Switch between User and Factory mode
    static const uint8_t GET_FIRMWARE_INFO = 21;              //!< Send firmware info
    static const uint8_t GET_UNIQUE_ID = 22;                  //!< Send unique ID

    static const uint8_t GET_DAILY_SALES_TOTAL_SUM = 25;      //!< Receive daily sales total sum
    static const uint8_t GET_DAILY_SALES_TAX_SUM = 26;        //!< Receive daily sales tax sum

    static const uint8_t SET_DAILY_SALES_TOTAL_SUM_RANGE_BY_INDEX = 27;       //!< Set range by index
    static const uint8_t SET_DAILY_SALES_TOTAL_SUM_RANGE_BY_DATE_TIME = 28;   //!< Set range by date/time
    static const uint8_t GET_DAILY_SALES_TOTAL_SUM_RANGE = 29;                //!< Receive daily sales total sum within range
    static const uint8_t GET_DAILY_SALES_TAX_SUM_RANGE = 30;                  //!< Receive daily sales tax sum within range

    static const uint8_t GET_LAST_ERROR_CODE = 255;           //!< Send last error code
public:
    // Error Codes
    //! Communication okay
    static const int8_t CMD_OK = 0;
    //! Authentication failed
    static const int8_t CMD_AUTHENTICATION_FAILED = -1;
    //! Not Authenticated
    static const int8_t CMD_NOT_AUTHENTICATED = -2;
    //! data incomplete
    static const int CMD_DATA_INCOMPLETE = -3;
    //! Invalid Argument
    static const int8_t CMD_ARGUMENT_INVALID = -4;
    //! No space left (Memory is full)
    static const int CMD_NO_SPACE_LEFT = -5;
    //! Not possible to set serials or code page twice
    static const int8_t CMD_FISCAL_NUMBER_AND_CODE_HAS_BEEN_ALREADY_SET = -6;
    //! Serials or code page not set (not possible to write entries)
    static const int8_t CMD_FISCAL_NUMBER_AND_CODE_NOT_SET = -7;
    //! sent command not supported in the current command state
    static const int8_t CMD_CANNOT_EXECUTE_THE_COMMAND_IN_THIS_STATE = -8;
    //! Memory communication error appears
    static const int8_t CMD_HARDWARE_FAULT = -9;
    //! Revolving Amount not set
    static const int8_t CMD_FISCAL_REVOLVING_AMOUNT_NOT_SET = -10;
      //! The given command is not defined or it is defined for the other direction
    static const int8_t CMD_UNKNOWN_COMMAND = -11;

  /**********************************************************/
  /*
    *  TYPEDEFS 
    */
  /**********************************************************/
public:
    //! switch AES cryption on/off
    static const bool CRYPTED = true; 

    //! fiscal code: minimum size
    static const int FISCAL_CODE_MIN_SIZE = 1;

    //! fiscal code: maximum size
    static const int FISCAL_CODE_MAX_SIZE = 15;

    //! fiscal number: minimum size
    static const int FISCAL_NUMBER_MIN_SIZE = 1;

    //! fiscal number: maximum size
    static const int FISCAL_NUMBER_MAX_SIZE = 15;

    //! fiscal revolving amount size
    static const int REVOLVING_AMOUNT_SIZE = 15;
    
    //! entry size
    static const int ENTRY_SIZE = 15;


  /**********************************************************/
  /*
    *  Methods
    */
  /**********************************************************/

  public:
    FlashManager ();
    ~FlashManager ();

    int32_t Init(FM_CONTEXT_t const *fm_ctx);
    bool IsInited();

    int SetFiscalCode(const uint8_t* pBuffer, int number_of_bytes);
    int GetFiscalCode(uint8_t* pBuffer, uint8_t &number_of_bytes);

    int SetFiscalNumber(const uint8_t* pBuffer, int number_of_bytes);
    int GetFiscalNumber(uint8_t* pBuffer, uint8_t &number_of_bytes);

    int SetEntry(const uint8_t* pBuffer, int number_of_bytes);
    int SetEntryNumber(uint32_t entry_number);
    int GetEntry(uint8_t* pBuffer, uint8_t &number_of_bytes);
    
    uint64_t GetDailySalesTotal();
    uint64_t GetDailySalesTaxTotal();

    int SetDailySalesTotalStartStop_Index (uint32_t start_idx, uint32_t stop_idx);
    int SetDailySalesTotalStartStop_DateTime (uint32_t start_date_time, uint32_t stop_date_time);
    uint64_t GetDailySalesTotal_Range();
    uint64_t GetDailySalesTaxTotal_Range();

    uint32_t GetNumberOfEntries();
    uint32_t GetEntrySpace();

    int SetFiscalRevolvingAmount(const uint8_t* pBuffer, int number_of_bytes);
    int GetFiscalRevolvingAmount(uint8_t* pBuffer, uint8_t &number_of_bytes);

    int SetMode(uint8_t mode);

    void ClearCompleteCard();
    int GetFlashStatus();
    bool GetFullStatus();

    str_fmInfo GetFmInfo();
  /**********************************************************/
  /*!
    *  MEMBER VARIABLES
    */
  /**********************************************************/

  private:
    //! Managed the internal flash content
    Configuration m_config;

    //! number of current entry to be get
    uint32_t m_entry_number;

    //! start index for GetDailySalesTotal_Range() and GetDailySalesTaxTotal_Range()
    int m_get_daily_sales_total_start_idx;

    //! stop index for GetDailySalesTotal_Range() and GetDailySalesTaxTotal_Range()
    int m_get_daily_sales_total_stop_idx;

    //! fiscal memory is in hardware fault (no correct flash communication possible)
    bool m_hardware_fault;
};

#endif
