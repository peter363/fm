package com.FisNano;

import java.io.IOError;
import java.io.IOException;
import java.util.Date;
import android.content.Context;
//import android.hardware.SerialManager;
//import android.hardware.SerialPort;
import android.util.Log;
import android.content.Context;

public class FiscalMemory {
    /**
     * 连接本地JNI动态库.
     */
    static {
        System.loadLibrary("FiscalMemory");
    }
    private static final String TAG = "FiscalMemory";

    /**
     * 构造函数
     *
     */
    public FiscalMemory(Context context) {
//        SerialInit(mSerialManager);
//        OpenDevice();
    }

    public boolean Open()
    {
        if(OpenDevice() >= 0)
        {
            Log.d(TAG, "Open Serial Success");
            return true;
        }

        Log.e(TAG, "Open Serial Error");
        return false;
    }

    public void Close()
    {
        Log.d(TAG, "Close Serial");
        CloseDevie();
    }

    public void Test(){
        SetFiscalCode(new byte[]{ 'F', 'i','s','c','a','l','C','o','d','e','1','2','3','4','5' });
        try
        {
            String mGetFirmwareInfo = GetFirmwareInfo();
            byte[] fis = GetFiscalCode();
            if(fis != null)
            {
                Log.d(TAG, "GetFiscalCode " + new String(fis));
            }
            if(mGetFirmwareInfo != null)
            {
                Log.d(TAG, "FirmwareInfo is " + mGetFirmwareInfo);
            }else
            {
                Log.e(TAG, "Test Fail");
            }
        }catch (IllegalArgumentException e)
        {
            e.printStackTrace();
        }
    }

    /* This Method Should be hide in Release Version */
    public int EraseCard()
    {
        return ClearCompleteCard();
    }

    public int SetDailySalesTotalSumRangeByDateTime(Date start, Date end)
    {
        long start_date_val = start.getMinutes()
                                + start.getHours() * 60
                                + start.getDay() * 24 * 60
                                + start.getMonth() * 24 * 60 * 31
                                + start.getYear() * 24 * 60 * 31 * 12;

        long end_date_val = end.getMinutes()
                + end.getHours() * 60
                + end.getDay() * 24 * 60
                + end.getMonth() * 24 * 60 * 31
                + end.getYear() * 24 * 60 * 31 * 12;

        return SetDailySalesTotalSumRangeByDateTime(start_date_val, end_date_val);
    }



//    private SerialPort mSerialPort;
//    private SerialManager mSerialManager;
    /**
     * 本地JNI层函数.打开串口设备.该函数的定义主体在 jni/serial_port.c文件中.
     *
     * @return 成功打开返回串口设备的{@link FileDescriptor}实例.否则返回null.
     */
    public native int OpenDevice();

    /**
     * 本地JNI层函数.关闭串口设备.该函数的定义主体在 jni/serial_port.c文件中.
     *
     * @return 成功关闭返回true,否则返回false.
     */
    public native boolean CloseDevie();


    /************************************
     *    Operation for Hardware        *
     ************************************/
    public native String GetFirmwareInfo();
    public native int ClearCompleteCard();
    public native int SoftwareReset();

    /************************************
    *    Operation for ConfigInfo       *
    ************************************/
    public native int SetFiscalNum(byte[] FiscalNum);
    public native int SetFiscalCode(byte[] FiscalCode);
    public native byte[] GetFiscalCode();
    public native byte[] GetFiscalNum();
    public native int SetMode(boolean EnableUserMode);
//    private native boolean GetFiscalNumberStatus();
//    private native boolean GetFiscalCodeStatus();
//    private native boolean GetFiscalRevolvingAmountStatus();
    public native boolean GetFullStatus();
//    private native void SetFullStatus();
    /************************************
     *    Operation for Entry           *
     ************************************/
    /* Entries that has used */
    public native int GetNumberOfEntries();

    /* Entries that not used */
    //private native int GetFreeEntries();

    /* Set Entry operate index */
    public native int SetEntryIndex(int index);

    /* Write Entry to EntryIndex */
    public native int SendDataOfEntry();

    /* Read Entry from EntryIndex */
    public native byte[] GetEntryData(int Num);

    public native int SetFiscalRevolingAmount();
    public native int GetFiscalRevolingAmount();

    /************************************
     *    Operation for DailyInfo       *
     ************************************/
    public native long GetDailySalesTotalSum();
    public native int SetDailySalesTotalSumRangeByIndex(int start_index, int end_idx);
    public native int SetDailySalesTotalSumRangeByDateTime(long start_date, long end_date);
    public native long GetDailySalesTotalSumRange();

    /************************************
     *    Operation for DailyTaxInfo       *
     ************************************/
    public native long GetDailySalesTaxSum();
    public native long GetDailySalesTaxSumRange();

    // Error Codes
    //! Communication okay
    public static int CMD_OK = 0;
    //! Authentication failed
    public static int CMD_AUTHENTICATION_FAILED = -1;
    //! Not Authenticated
    public static int CMD_NOT_AUTHENTICATED = -2;
    //! data incomplete
    public static int CMD_DATA_INCOMPLETE = -3;
    //! Invalid Argument
    public static int CMD_ARGUMENT_INVALID = -4;
    //! No space left (Memory is full)
    public static int CMD_NO_SPACE_LEFT = -5;
    //! Not possible to set serials or code page twice
    public static int CMD_FISCAL_NUMBER_AND_CODE_HAS_BEEN_ALREADY_SET = -6;
    //! Serials or code page not set (not possible to write entries)
    public static int CMD_FISCAL_NUMBER_AND_CODE_NOT_SET = -7;
    //! sent command not supported in the current command state
    public static int CMD_CANNOT_EXECUTE_THE_COMMAND_IN_THIS_STATE = -8;
    //! Memory communication error appears
    public static int CMD_HARDWARE_FAULT = -9;
    //! Revolving Amount not set
    public static int CMD_FISCAL_REVOLVING_AMOUNT_NOT_SET = -10;
    //! The given command is not defined or it is defined for the other direction
    public static int CMD_UNKNOWN_COMMAND = -11;
}
