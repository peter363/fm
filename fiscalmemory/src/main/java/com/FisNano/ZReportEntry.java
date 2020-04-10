package com.FisNano;

/**
 * @Description:
 * @Author: zys
 * @CreateDate: 2020/4/10 10:16
 * @Version: 1.0
 */
public class ZReportEntry {

    private static final String TAG = "ZReportEntry";

    private static final int YEAR_BASELINE = 2000;

    int sales_total;//in cent

    int sales_tax;//in cent

    int year;  //Year(“0” equals to “2000”)

    int month;

    int day;

    int hour;

    int minute;

    short serial_number;

    byte crc8;

    public int getSales_total() {
        return sales_total;
    }

    public void setSales_total(int sales_total) {
        this.sales_total = sales_total;
    }

    public int getSales_tax() {
        return sales_tax;
    }

    public void setSales_tax(int sales_tax) {
        this.sales_tax = sales_tax;
    }

    public int getYear() {
        return year;
    }

    public void setYear(int year) {
        if (year >= YEAR_BASELINE) {
            this.year = year - YEAR_BASELINE;
        }
    }

    public int getMonth() {
        return month;
    }

    public void setMonth(int month) {
        this.month = month;
    }

    public int getDay() {
        return day;
    }

    public void setDay(int day) {
        this.day = day;
    }

    public int getHour() {
        return hour;
    }

    public void setHour(int hour) {
        this.hour = hour;
    }

    public int getMinute() {
        return minute;
    }

    public void setMinute(int minute) {
        this.minute = minute;
    }

    public short getSerial_number() {
        return serial_number;
    }

    public void setSerial_number(short serial_number) {
        this.serial_number = serial_number;
    }

    public static ZReportEntry parseEntry(byte[] data) {
        ZReportEntry zReportEntry = new ZReportEntry();

        byte[] salesTotalBytes = new byte[4];
        System.arraycopy(data, 0, salesTotalBytes, 0, 4);
        zReportEntry.sales_total = bytes2int(salesTotalBytes);

        byte[] salesTaxBytes = new byte[4];
        System.arraycopy(data, 4, salesTaxBytes, 0, 4);
        zReportEntry.sales_tax = bytes2int(salesTaxBytes);

        byte yearBt = data[8];
        zReportEntry.year = yearBt + YEAR_BASELINE;

        byte[] dateAndTime = new byte[3];
        System.arraycopy(data, 9, dateAndTime, 0, 3);
        zReportEntry.month = dateAndTime[0] & 0x0F;
        zReportEntry.day = ((dateAndTime[1] & 0x01) << 4) | (dateAndTime[0] >> 4 & 0x0F);
        zReportEntry.hour = (dateAndTime[1] >> 1) & 0x1F;
        zReportEntry.minute = ((dateAndTime[1] >> 2) & 0x30) | dateAndTime[2] & 0x0f;

        byte[] serialNumberBytes = new byte[2];
        System.arraycopy(data, 12, serialNumberBytes, 0, 2);
        zReportEntry.serial_number = bytes2Short(serialNumberBytes);

        zReportEntry.crc8 = data[14];

        return zReportEntry;
    }

    public byte[] getZReportData() {
        byte[] rtnData = new byte[15];

        byte[] salesTotalBytes = int2Bytes(sales_total);
        System.arraycopy(salesTotalBytes, 0, rtnData, 0, 4);

        byte[] salesTaxBytes = int2Bytes(sales_tax);
        System.arraycopy(salesTaxBytes, 0, rtnData, 4, 4);

        rtnData[8] = (byte) (year & 0xFF);

        byte[] dateAndTime = new byte[3];
        dateAndTime[0] = (byte) ((month & 0x0F) | (day << 4 & 0xF0));
        dateAndTime[1] = (byte) (((day >> 4) & 0x01) | ((hour << 1) & 0x3E) | ((minute << 2) & 0xC0));
        dateAndTime[2] = (byte) (minute & 0x0F);
        System.arraycopy(dateAndTime, 0, rtnData, 9, 3);

        byte[] serialNumberBytes = short2Bytes(serial_number);
        System.arraycopy(serialNumberBytes, 0, rtnData, 12, 2);

        byte[] crcCalcData = new byte[14];
        System.arraycopy(rtnData, 0, crcCalcData, 0, 14);

        crc8 = CRC8Util.calcCrc8(crcCalcData);

        rtnData[14] = crc8;

        return rtnData;
    }

    private static byte[] int2Bytes(int n) {
        byte[] b = new byte[4];
        b[0] = (byte) (n & 0xff);
        b[1] = (byte) (n >> 8 & 0xff);
        b[2] = (byte) (n >> 16 & 0xff);
        b[3] = (byte) (n >> 24 & 0xff);
        return b;
    }

    private static int bytes2int(byte[] b) {
        int res = 0;
        for (int i = 0; i < b.length; i++) {
            res += (b[i] & 0xff) << (i * 8);
        }
        return res;
    }


    private static byte[] short2Bytes(short n) {
        byte[] b = new byte[2];
        b[0] = (byte) (n & 0xff);
        b[1] = (byte) (n >> 8 & 0xff);
        return b;
    }

    private static short bytes2Short(byte[] b) {
        short res = 0;
        for (int i = 0; i < b.length; i++) {
            res += (b[i] & 0xff) << (i * 8);
        }
        return res;
    }

    @Override
    public String toString() {
        return "ZReportEntry{" +
                "sales_total=" + sales_total +
                ", sales_tax=" + sales_tax +
                ", year=" + year +
                ", month=" + month +
                ", day=" + day +
                ", hour=" + hour +
                ", minute=" + minute +
                ", serial_number=" + serial_number +
                ", crc8=" + crc8 +
                '}';
    }
}
