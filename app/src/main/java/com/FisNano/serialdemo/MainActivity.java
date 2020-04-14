package com.FisNano.serialdemo;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.DatePickerDialog;
import android.app.TimePickerDialog;
import android.os.Bundle;

import com.FisNano.FiscalMemory;
import com.FisNano.ZReportEntry;

import android.os.Handler;
import android.os.Message;
import android.text.Editable;
import android.text.TextUtils;
import android.text.TextWatcher;
import android.util.Log;
import android.view.View;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.Button;
import android.widget.DatePicker;
import android.widget.EditText;
import android.widget.ScrollView;
import android.widget.TextView;
import android.content.Context;
import android.widget.TimePicker;
import android.widget.Toast;

import org.w3c.dom.Text;

import java.io.UnsupportedEncodingException;
import java.util.Calendar;
import java.util.Date;
//import android.hardware.SerialManager;


public class MainActivity extends Activity {
    static String TAG = "FiscalMem";

    TextView m_ConsoleText;
    ScrollView m_ScrollView;
    FiscalMemory m_FiscalFmemory;

    Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            if (0 == msg.what) {
                if (openingDialog != null) {
                    openingDialog.dismiss();
                    openingDialog = null;
                }
                String str = (String) msg.obj;
                m_ConsoleText.setText(m_ConsoleText.getText() + str + "\n");
                m_ScrollView.fullScroll(ScrollView.FOCUS_DOWN);
            }

        }
    };
    private EditText mEtFiscalcode;
    private EditText mEtFiscalNum;
    private EditText mEtRevolvingAmount;
    private EditText mEtTotalTax;
    private TextView mTvDate;
    private TextView mTvTime;
    private TextView mTvDailySalesDateStart;
    private TextView mTvDailySalesTimeStart;
    private TextView mTvDailySalesDateEnd;
    private TextView mTvDailySalesTimeEnd;
    private EditText mEtStart2EndIndex;
    private AlertDialog openingDialog;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_main);

        m_ConsoleText = (TextView) findViewById(R.id.LogText);
        m_ScrollView = (ScrollView) findViewById(R.id.up_scrollview);

        mEtFiscalcode = (EditText) findViewById(R.id.et_fiscalcode);
        mEtFiscalNum = (EditText) findViewById(R.id.et_fiscalnum);
        mEtRevolvingAmount = (EditText) findViewById(R.id.et_revolving_amount);

        mEtTotalTax = (EditText) findViewById(R.id.et_sales_total_tax);
        mTvDate = (TextView) findViewById(R.id.tv_date);
        mTvTime = (TextView) findViewById(R.id.tv_time);

        mTvDailySalesDateStart = (TextView) findViewById(R.id.tv_dailysales_date_start);
        mTvDailySalesTimeStart = (TextView) findViewById(R.id.tv_dailysales_time_start);

        mTvDailySalesDateEnd = (TextView) findViewById(R.id.tv_dailysales_date_end);
        mTvDailySalesTimeEnd = (TextView) findViewById(R.id.tv_dailysales_time_end);

        mEtStart2EndIndex = (EditText) findViewById(R.id.et_start_to_end_index);
        mEtStart2EndIndex.addTextChangedListener(new TextWatcher() {
            @Override
            public void beforeTextChanged(CharSequence charSequence, int i, int i1, int i2) {

            }

            @Override
            public void onTextChanged(CharSequence charSequence, int i, int i1, int i2) {

            }

            @Override
            public void afterTextChanged(Editable editable) {
                if (editable.length() != 0) {
                    mTvDailySalesDateStart.setText("");
                    mTvDailySalesTimeStart.setText("");
                    mTvDailySalesDateEnd.setText("");
                    mTvDailySalesTimeEnd.setText("");
                }
            }
        });


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

    public void showDatePickerDialog(Activity activity, final TextView tv, Calendar calendar) {
        new DatePickerDialog(activity,
                new DatePickerDialog.OnDateSetListener() {
                    @Override
                    public void onDateSet(DatePicker datePicker,
                                          int year, int month, int dayOfMonth) {
                        tv.setText(getString(R.string.fmt_date, year, month + 1, dayOfMonth));
                    }
                },
                calendar.get(Calendar.YEAR),
                calendar.get(Calendar.MONTH),
                calendar.get(Calendar.DAY_OF_MONTH)).show();
    }

    public void showTimePickerDialog(Activity activity, final TextView tv, Calendar calendar) {
        new TimePickerDialog(activity,
                new TimePickerDialog.OnTimeSetListener() {
                    @Override
                    public void onTimeSet(TimePicker timePicker, int hourOfDay, int minute) {
                        tv.setText(getString(R.string.fmt_time, hourOfDay, minute));
                    }
                },
                calendar.get(Calendar.HOUR),
                calendar.get(Calendar.MINUTE),
                true).show();
    }

    int testStr = 0;

    public void onClick(View v) {
        int ret;
        String OutStr = "";
        Log.d(TAG, "V.id = " + v.getId());
        switch (v.getId()) {
            case R.id.btn_open:

                OutStr += "Opening...\n";

                AlertDialog.Builder builder = new AlertDialog.Builder(MainActivity.this);
                builder.setTitle("Tip:");
                builder.setMessage("Opening...");
                builder.setCancelable(false);
                openingDialog = builder.create();
                openingDialog.show();

                new Thread() {
                    @Override
                    public void run() {
                        String tpmStr = "Open FiscalFmemory: ";
                        boolean result = m_FiscalFmemory.Open();
                        if (result) {
                            tpmStr += "Success";
                        } else {
                            tpmStr += "Fail";
                        }

                        Message msg = mHandler.obtainMessage();
                        msg.what = 0;
                        msg.obj = tpmStr;

                        mHandler.sendMessage(msg);
                    }
                }.start();
                break;

            case R.id.btn_close:
                OutStr = "Close FiscalFmemory\n";
                m_FiscalFmemory.Close();
                break;

            case R.id.btn_setmode:
//                m_FiscalFmemory.SetMode(false);
                break;

            case R.id.btn_write_fiscal_code:

                String fiscalCode = mEtFiscalcode.getText().toString();

                ret = m_FiscalFmemory.SetFiscalCode(fiscalCode.getBytes());

                OutStr += (ret == FiscalMemory.CMD_OK) ? "SetFiscalCode:Success" : "SetFiscalCode:Fail";

                OutStr += "\n";

                break;
            case R.id.btn_write_fiscal_num:

                String fiscalNum = mEtFiscalNum.getText().toString();

                ret = m_FiscalFmemory.SetFiscalNumber(fiscalNum.getBytes());

                OutStr += (ret == FiscalMemory.CMD_OK) ? "SetFiscalNumber:Success" : "SetFiscalNumber:Fail";

                OutStr += "\n";

                break;

            case R.id.btn_read_fiscal_code_and_number:
                try {
                    byte[] fis = m_FiscalFmemory.GetFiscalCode();
                    if (fis != null) {
                        OutStr += "GetFiscalCode:" + AsciiToString(fis) + "\r\n";
                    }
                    byte[] fisnum = m_FiscalFmemory.GetFiscalNumber();
                    if (fisnum != null) {
                        OutStr += "GetFiscalNum:" + AsciiToString(fisnum) + "\r\n";
                    }

                } catch (IllegalArgumentException | UnsupportedEncodingException e) {
                    e.printStackTrace();
                }

                break;
            case R.id.btn_read_firmwareinfo:
                String mGetFirmwareInfo = m_FiscalFmemory.GetFirmwareInfo();
                if (mGetFirmwareInfo != null) {
                    OutStr += "FirmwareInfo is:" + mGetFirmwareInfo;
                } else {
                    OutStr += "FirmwareInfo Test Fail";
                }

                OutStr += "\n";
                break;

            case R.id.btn_erase:
                OutStr = "\nErase FiscalFmemory: ";

                ret = m_FiscalFmemory.EraseCard();
                if (ret >= 0) {
                    OutStr += "Success";
                } else {
                    OutStr += "Fail " + ret;
                }
                OutStr += "\n";
                break;

            case R.id.btn_write_z_report:

                String totalTax = mEtTotalTax.getText().toString();
                String date = mTvDate.getText().toString();
                String time = mTvTime.getText().toString();

                if (TextUtils.isEmpty(totalTax) || TextUtils.isEmpty(date) || TextUtils.isEmpty(time)) {
                    return;
                }

                String[] split = totalTax.split(",");
                int total = Integer.parseInt(split[0]);
                int tax = Integer.parseInt(split[1]);
                short serialNum = Short.parseShort(split[2]);

                String[] split1 = date.split(",");
                int year = Integer.parseInt(split1[0]);
                int month = Integer.parseInt(split1[1]);
                short day = Short.parseShort(split1[2]);

                String[] split2 = time.split(",");
                int hour = Integer.parseInt(split2[0]);
                int minute = Integer.parseInt(split2[1]);

                ZReportEntry zReportEntry = new ZReportEntry();
                zReportEntry.setSales_total(total);
                zReportEntry.setSales_tax(tax);
                zReportEntry.setYear(year);
                zReportEntry.setMonth(month);
                zReportEntry.setDay(day);
                zReportEntry.setHour(hour);
                zReportEntry.setMinute(minute);
                zReportEntry.setSerial_number(serialNum);

                byte[] zReportData = zReportEntry.getZReportData();

                android.util.Log.e(TAG, "[zys-->] write:" + bytes2BinaryStr(zReportData));

                ret = m_FiscalFmemory.SetEntryData(zReportData);

                OutStr += (ret == FiscalMemory.CMD_OK) ? "Success" : "Fail";

                break;

            case R.id.btn_read_z_report:

//                int i = m_FiscalFmemory.SetEntryIndex(0);
                byte[] bytes = m_FiscalFmemory.GetEntryData(0);

                android.util.Log.e(TAG, "[zys-->] read:" + bytes2BinaryStr(bytes));

                ZReportEntry zReportEntry1 = ZReportEntry.parseEntry(bytes);

                OutStr += "\n" + zReportEntry1.toString();

                break;

            case R.id.tv_date:
                showDatePickerDialog(this, mTvDate, Calendar.getInstance());
                break;

            case R.id.tv_time:
                showTimePickerDialog(this, mTvTime, Calendar.getInstance());
                break;

            case R.id.btn_getfullstatus:

                boolean b = m_FiscalFmemory.GetFullStatus();

                OutStr += b ? "FiscalMemory is full\n" : "FiscalMemory is have space\n";

                break;

            case R.id.btn_getfreeentries:

                int i1 = m_FiscalFmemory.GetFreeEntries();

                OutStr += "Z Report Free space:" + i1;

                break;

            case R.id.btn_getnumberofentries:

                int i = m_FiscalFmemory.GetNumberOfEntries();

                OutStr += "Z Report number:" + i;

                OutStr += "\n";
                break;

            case R.id.btn_set_revolving_amount:

                String s = mEtRevolvingAmount.getText().toString();
                if (TextUtils.isEmpty(s)) {
                    return;
                }

                ret = m_FiscalFmemory.SetFiscalRevolingAmount(s.getBytes());
                OutStr += (ret == FiscalMemory.CMD_OK) ? "SetFiscalRevolingAmount:Success" : "SetFiscalRevolingAmount:Fail";

                OutStr += "\n";

                break;
            case R.id.btn_get_revolving_amount:

                byte[] revolingAmountBytes = m_FiscalFmemory.GetFiscalRevolingAmount();
                if (revolingAmountBytes != null) {
                    OutStr += "GetFiscalRevolingAmount : " + (revolingAmountBytes.toString());
                } else {
                    OutStr += "GetFiscalRevolingAmount : Empty";
                }
                OutStr += "\n";
                break;

            case R.id.btn_get_daily_sales_totalsum:

                long l = m_FiscalFmemory.GetDailySalesTotalSum();
                OutStr += "GetDailySalesTotalSum : " + l;
                OutStr += "\n";
                break;

            case R.id.btn_getdailysalestaxsum:

                long l3 = m_FiscalFmemory.GetDailySalesTaxSum();
                OutStr += "GetDailySalesTaxSum : " + l3;
                OutStr += "\n";
                break;

            case R.id.btn_getdailysalestotalsumrange:

                SetDailySalesTotalSumRange();

                long l1 = m_FiscalFmemory.GetDailySalesTotalSumRange();
                OutStr += "GetDailySalesTotalSumRange : " + l1;
                OutStr += "\n";
                break;

            case R.id.btn_getdailysalestaxsumrange:

                SetDailySalesTotalSumRange();

                long l2 = m_FiscalFmemory.GetDailySalesTaxSumRange();
                OutStr += "GetDailySalesTaxSumRange : " + l2;
                OutStr += "\n";
                break;

            case R.id.tv_dailysales_date_start:
                mEtStart2EndIndex.setText("");
                showDatePickerDialog(this, mTvDailySalesDateStart, Calendar.getInstance());

                return;

            case R.id.tv_dailysales_time_start:
                mEtStart2EndIndex.setText("");
                mEtStart2EndIndex.setText("");
                showTimePickerDialog(this, mTvDailySalesTimeStart, Calendar.getInstance());

                return;

            case R.id.tv_dailysales_date_end:
                mEtStart2EndIndex.setText("");
                showDatePickerDialog(this, mTvDailySalesDateEnd, Calendar.getInstance());

                return;

            case R.id.tv_dailysales_time_end:
                mEtStart2EndIndex.setText("");
                showTimePickerDialog(this, mTvDailySalesTimeEnd, Calendar.getInstance());

                break;

            default:
                return;
        }

        m_ConsoleText.setText(m_ConsoleText.getText() + OutStr + "\r\n");
        m_ScrollView.fullScroll(ScrollView.FOCUS_DOWN);
    }

    private void SetDailySalesTotalSumRange() {
        String start2endIndex = mEtStart2EndIndex.getText().toString();
        if (TextUtils.isEmpty(start2endIndex)) {
            String s1 = mTvDailySalesDateStart.getText().toString();
            String[] split1 = s1.split(",");
            String s2 = mTvDailySalesTimeStart.getText().toString();
            String[] split2 = s2.split(",");

            int s_year = Integer.parseInt(split1[0]);
            int s_month = Integer.parseInt(split1[1]);
            int s_day = Integer.parseInt(split1[2]);

            int s_hour = Integer.parseInt(split2[0]);
            int s_min = Integer.parseInt(split2[1]);

            Date startDate = new Date(s_year, s_month, s_day, s_hour, s_min);

            String s3 = mTvDailySalesDateEnd.getText().toString();
            String[] split3 = s3.split(",");
            String s4 = mTvDailySalesTimeEnd.getText().toString();
            String[] split4 = s4.split(",");

            int e_year = Integer.parseInt(split3[0]);
            int e_month = Integer.parseInt(split3[1]);
            int e_day = Integer.parseInt(split3[2]);

            int e_hour = Integer.parseInt(split4[0]);
            int e_min = Integer.parseInt(split4[1]);

            Date endDate = new Date(e_year, e_month, e_day, e_hour, e_min);

            m_FiscalFmemory.SetDailySalesTotalSumRangeByDateTime(startDate, endDate);
        } else {
            String[] split3 = start2endIndex.split(",");

            int start_index = Integer.parseInt(split3[0]);
            int end_index = Integer.parseInt(split3[1]);

            m_FiscalFmemory.SetDailySalesTotalSumRangeByIndex(start_index, end_index);
        }

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
