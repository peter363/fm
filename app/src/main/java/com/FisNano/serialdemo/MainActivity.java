package com.FisNano.serialdemo;

import android.app.Activity;
import android.os.Bundle;

import com.FisNano.FiscalMemory;
import com.FisNano.ZReportEntry;

import androidx.appcompat.app.AppCompatActivity;

import android.util.Log;
import android.view.View;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.Button;
import android.widget.ScrollView;
import android.widget.TextView;
import android.content.Context;

import java.io.UnsupportedEncodingException;
import java.math.BigInteger;
//import android.hardware.SerialManager;


public class MainActivity extends Activity {
    static String TAG = "FiscalMem";

    Button m_OpenBtn, m_CloseBtn;
    TextView m_ConsoleText;
    ScrollView m_ScrollView;
    FiscalMemory m_FiscalFmemory;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_main);

        m_ConsoleText = (TextView) findViewById(R.id.LogText);
        m_OpenBtn = (Button) findViewById(R.id.btn_open);
        m_CloseBtn = (Button) findViewById(R.id.btn_close);
        m_ScrollView = (ScrollView) findViewById(R.id.up_scrollview);


        Context context = getApplicationContext();
//        SerialManager mSerialManager = (SerialManager) getSystemService("serial");
        m_FiscalFmemory = new FiscalMemory(context);

    }


    @Override
    public void onResume() {
        super.onResume();
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }

    /**
     * Ascii转换为字符串
     *
     * @param value
     * @return
     */
    public static String AsciiToString(byte[] value) throws UnsupportedEncodingException {
        return new String(value, "ISO-8859-1");
    }

    public void onClick(View v) {
        int ret;
        String OutStr = "";
        Log.d(TAG, "V.id = " + v.getId());
        switch (v.getId()) {
            case R.id.btn_open:
                m_ConsoleText.setText(m_ConsoleText.getText() + "Opening..." + "\r\n");
                m_ScrollView.fullScroll(ScrollView.FOCUS_DOWN);
                OutStr = "Open FiscalFmemory: ";
                boolean result = m_FiscalFmemory.Open();
                if (result) {
                    OutStr += "Success";
                } else {
                    OutStr += "Fail";
                }
                break;

            case R.id.btn_close:
                OutStr = "Close FiscalFmemory";
                m_FiscalFmemory.Close();
                break;

            case R.id.btn_write:
                OutStr = "Write Test FiscalFmemory\r\n";
                OutStr = "Write Test SetFiscalCode:";
                ret = m_FiscalFmemory
                        .SetFiscalCode(new byte[]{'F', 'i', 's', 'c', 'a', 'l', 'C', 'o', 'd', 'e', '1', '2', '3', '4', '5'});
                OutStr += (ret == FiscalMemory.CMD_OK) ? "Success" : "Fail";

                ret = m_FiscalFmemory.SetFiscalNumber(new byte[]{'F', 'i', 's', 'c', 'a', 'l', 'N', 'u', 'm', '3', '3', '2', '3', '4', '5'});
                OutStr += (ret == FiscalMemory.CMD_OK) ? "Success" : "Fail";

                break;

            case R.id.btn_read:
                try {
                    String mGetFirmwareInfo = m_FiscalFmemory.GetFirmwareInfo();
                    byte[] fis = m_FiscalFmemory.GetFiscalCode();
                    if (fis != null) {
                        OutStr += "GetFiscalCode " + AsciiToString(fis) + "\r\n";
                        Log.d(TAG, "GetFiscalCode " + AsciiToString(fis));
                    }
                    byte[] fisnum = m_FiscalFmemory.GetFiscalNumber();
                    if (fisnum != null) {
                        OutStr += "GetFiscalNum " + AsciiToString(fisnum) + "\r\n";
                        Log.d(TAG, "GetFiscalNum " + AsciiToString(fisnum));
                    }
                    if (mGetFirmwareInfo != null) {
                        OutStr += "FirmwareInfo is " + mGetFirmwareInfo + "\r\n";
                        Log.d(TAG, "FirmwareInfo is " + mGetFirmwareInfo);
                    } else {
                        OutStr += "Test Fail\r\n";
                        Log.e(TAG, "Test Fail");
                    }
                } catch (IllegalArgumentException | UnsupportedEncodingException e) {
                    e.printStackTrace();
                }
                break;

            case R.id.btn_erase:
                OutStr = "Erase FiscalFmemory: ";
                ret = m_FiscalFmemory.EraseCard();
                if (ret >= 0) {
                    OutStr += "Success";
                } else {
                    OutStr += "Fail " + ret;
                }
                break;

            case R.id.btn_reset:
                OutStr = "SoftwareReset FiscalFmemory: ";
                ret = m_FiscalFmemory.SoftwareReset();
                OutStr += (ret == FiscalMemory.CMD_OK) ? "Success" : "Fail";
                break;
            case R.id.btn_write_z_report:

                ZReportEntry zReportEntry = new ZReportEntry();
                zReportEntry.setSales_total(445566);
                zReportEntry.setSales_tax(778899);
                zReportEntry.setYear(2059);
                zReportEntry.setMonth(5);
                zReportEntry.setDay(6);
                zReportEntry.setHour(2);
                zReportEntry.setMinute(5);
                short serialNumber = 12345;
                zReportEntry.setSerial_number(serialNumber);

                byte[] zReportData = zReportEntry.getZReportData();

                android.util.Log.e(TAG, "[zys-->] write:" + bytes2BinaryStr(zReportData));

                ret = m_FiscalFmemory.SetEntryData(zReportData);

                OutStr += (ret == FiscalMemory.CMD_OK) ? "Success" : "Fail";

                break;

            case R.id.btn_read_z_report:

//                int number = m_FiscalFmemory.GetNumberOfEntries();
//                OutStr += "GetNumberOfEntries:" + number;

                byte[] bytes = m_FiscalFmemory.GetEntryData(0);

                android.util.Log.e(TAG, "[zys-->] read:" + bytes2BinaryStr(bytes));

                ZReportEntry zReportEntry1 = ZReportEntry.parseEntry(bytes);

                OutStr += zReportEntry1.toString();


//                long totalsum = m_FiscalFmemory.GetDailySalesTotalSum();
//                long taxsum = m_FiscalFmemory.GetDailySalesTaxSum();
//
//                OutStr += "totalsum:" + totalsum + ", taxsum:" + taxsum;

                break;

            default:
                return;
        }

        m_ConsoleText.setText(m_ConsoleText.getText() + OutStr + "\r\n");
        m_ScrollView.fullScroll(ScrollView.FOCUS_DOWN);
    }

    public static String bytes2BinaryStr(byte[] bArray) {
        String outStr = "";
        int pos = 0;
        for (byte b : bArray) {
            //高四位
            pos = (b & 0xF0) >> 4;
            outStr += binaryArray[pos];
            //低四位
            pos = b & 0x0F;
            outStr += binaryArray[pos];

            outStr += " ";
        }
        return outStr;

    }

    private static String[] binaryArray =
            {"0000", "0001", "0010", "0011",
                    "0100", "0101", "0110", "0111",
                    "1000", "1001", "1010", "1011",
                    "1100", "1101", "1110", "1111"};
}
