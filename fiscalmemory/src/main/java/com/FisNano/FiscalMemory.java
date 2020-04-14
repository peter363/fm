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
     */
    public FiscalMemory(Context context) {
//        SerialInit(mSerialManager);
//        OpenDevice();
    }

    public boolean Open() {
        if (OpenDevice() >= 0) {
            Log.d(TAG, "Open Serial Success");
            return true;
        }

        Log.e(TAG, "Open Serial Error");
        return false;
    }

    public void Close() {
        Log.d(TAG, "Close Serial");
        CloseDevie();
    }

    public void Test() {
        SetFiscalCode(new byte[]{'F', 'i', 's', 'c', 'a', 'l', 'C', 'o', 'd', 'e', '1', '2', '3', '4', '5'});
        try {
            String mGetFirmwareInfo = GetFirmwareInfo();
            byte[] fis = GetFiscalCode();
            if (fis != null) {
                Log.d(TAG, "GetFiscalCode " + new String(fis));
            }
            if (mGetFirmwareInfo != null) {
                Log.d(TAG, "FirmwareInfo is " + mGetFirmwareInfo);
            } else {
                Log.e(TAG, "Test Fail");
            }
        } catch (IllegalArgumentException e) {
            e.printStackTrace();
        }
    }

    /* This Method Should be hide in Release Version */
    public int EraseCard() {
        return ClearCompleteCard();
    }

    public int SetDailySalesTotalSumRangeByDateTime(Date start, Date end) {
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
     * @return 成功关闭返回true, 否则返回false.
     */
    public native boolean CloseDevie();


    /************************************
     *    Operation for Hardware        *
     ************************************/
    /**
     * 获取固件信息
     *
     * @return 固件信息字符串，包含总容量（实际物理容量），读/写/擦除块大小
     */
    public native String GetFirmwareInfo();

    /**
     * 清理FM中的所有数据，该接口仅供调试使用
     *
     * @return 返回值为Error Codes
     */
    private native int ClearCompleteCard();

    /**
     * 对FM记忆体实行软件复位
     *
     * @return 返回值为Error Codes
     */
    public native int SoftwareReset();

    /************************************
     *    Operation for ConfigInfo       *
     ************************************/
    /**
     * 设置Fiscal Num
     *
     * @return 返回值为Error Codes
     */
    public native int SetFiscalNumber(byte[] FiscalNum);

    /**
     * 设置Fiscal Code
     *
     * @return 返回值为Error Codes
     */
    public native int SetFiscalCode(byte[] FiscalCode);

    /**
     * 获取已经写入的Fiscal Code,如果未写入返回空值
     *
     * @return FiscalCode数组
     */
    public native byte[] GetFiscalCode();

    /**
     * 获取已经写入的Fiscal Num,如果未写入返回空值
     *
     * @return FiscalNum数组
     */
    public native byte[] GetFiscalNumber();

    /**
     * 使能用户模式
     *
     * @return ErrorCode
     */
    public native int SetMode(boolean EnableUserMode);

    /**
     * 判断卡是否已满
     *
     * @return true:卡已满  false:卡未满
     */
    public native boolean GetFullStatus();

    /************************************
     *    Operation for Entry           *
     ************************************/
    /**
     * 获取已经写入的Z报表数量
     *
     * @return 已经写入的Z报表数量
     */
    public native int GetNumberOfEntries();

    /**
     * 获取Z报表剩余空间
     *
     * @return Z报表剩余空间
     */
    public native int GetFreeEntries();

    /**
     * 设置当前的Z报表索引值
     *
     * @param index Z报表索引值
     * @return ErrCode
     */
    public native int SetEntryIndex(int index);

    /**
     * 根据Num设置的索引值，获取Z报表数据
     *
     * @param Num Z报表索引值
     * @return Z报表数据
     */
    public native byte[] GetEntryData(int Num);

    /**
     * 根据Num设置的索引值，写入Z报表数据
     *
     * @param entryData Z报表数据
     * @return ErrCode
     */
    public native int SetEntryData(byte[] entryData);

    public native int SetFiscalRevolingAmount(byte[] RevolingAmount);

    public native byte[] GetFiscalRevolingAmount();

    /************************************
     *    Operation for DailyInfo       *
     ************************************/
    /**
     * 获取日销售额总和,该函数根据目前Z报表已有数据计算
     *
     * @return 日销售额总和
     */
    public native long GetDailySalesTotalSum();

    /**
     * 配合GetDailySalesTotalSumRange及GetDailySalesTaxSumRange函数，
     * 设置参数索引值获取区间总和
     * 该函数根据目前Z报表已有数据计算
     *
     * @param start_index 区间起始索引
     * @param end_idx     区间结束索引
     * @return ErrCode
     */
    public native int SetDailySalesTotalSumRangeByIndex(int start_index, int end_idx);

    /**
     * 配合GetDailySalesTotalSumRange及GetDailySalesTaxSumRange函数，
     * 设置参数索引值获取区间总和
     * 该函数根据目前Z报表已有数据计算
     *
     * @param start_date 区间起始索引
     * @param end_date   区间结束索引
     * @return ErrCode
     */
    private native int SetDailySalesTotalSumRangeByDateTime(long start_date, long end_date);

    /**
     * 获取区间起始索引计算日销售额区间和,该函数根据目前Z报表已有数据计算
     * 该函数需要配合SetDailySalesTotalSumRangeByIndex及SetDailySalesTotalSumRangeByDateTime使用     *
     *
     * @return 日销售额区间总和
     */
    public native long GetDailySalesTotalSumRange();

    /************************************
     *    Operation for DailyTaxInfo       *
     ************************************/
    /**
     * 获取日销售额总和,该函数根据目前Z报表已有数据计算
     *
     * @return 日销售税额总和
     */
    public native long GetDailySalesTaxSum();

    /**
     * 获取区间起始索引计算日销售税额区间和,该函数根据目前Z报表已有数据计算
     * 该函数需要配合SetDailySalesTotalSumRangeByIndex及SetDailySalesTotalSumRangeByDateTime使用     *
     *
     * @return 日销售税额区间总和
     */
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
