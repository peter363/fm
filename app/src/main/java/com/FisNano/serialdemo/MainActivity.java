package com.FisNano.serialdemo;

import android.annotation.SuppressLint;
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
import android.widget.DatePicker;
import android.widget.EditText;
import android.widget.ScrollView;
import android.widget.TextView;
import android.content.Context;
import android.widget.TimePicker;
import android.widget.Toast;

import java.io.UnsupportedEncodingException;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.Date;
import java.util.List;
//import android.hardware.SerialManager;


public class MainActivity extends Activity {
    static String TAG = "FiscalMem";

    private static final int HANDLER_WRITE_SUCCEED = 1;
    private static final int HANDLER_WRITE_COMPLETE = 2;
    private static final int HANDLER_READ_SUCCEED = 3;
    private static final int HANDLER_READ_COMPLETE = 4;
    private static final int HANDLER_READ_COMPARE = 5;
    private static final int HANDLER_READ_COMPARE_COMPLETE = 6;
    private static final int HANDLER_LOOPFUNC_PRINT_CONTENT = 7;

    TextView m_ConsoleText;
    ScrollView m_ScrollView;
    FiscalMemory m_FiscalFmemory;
    String oriContent = null;

    int loopFuncCount = 0;

    List<ZReportEntry> cacheZReportEntrys = new ArrayList<ZReportEntry>();

    Handler mHandler = new Handler() {
        @SuppressLint("HandlerLeak")
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
            } else if (HANDLER_WRITE_SUCCEED == msg.what) {

                oriContent = m_ConsoleText.getText().toString();

                ZReportEntry zReportEntry = (ZReportEntry) msg.obj;

                m_ConsoleText.setText(oriContent + getString(R.string.write_zreport_test, zReportEntry.getTest_index() + 1, zReportEntry.toString()));
                m_ScrollView.fullScroll(ScrollView.FOCUS_DOWN);
                m_ScrollView.fullScroll(ScrollView.FOCUS_DOWN);
            } else if (HANDLER_WRITE_COMPLETE == msg.what) {
                loopCount = -1;
                oriContent = m_ConsoleText.getText().toString();
                m_ConsoleText.setText(oriContent + "Complete.\n");
            } else if (HANDLER_READ_SUCCEED == msg.what) {
                oriContent = m_ConsoleText.getText().toString();
                ZReportEntry zReportEntry = (ZReportEntry) msg.obj;
                m_ConsoleText.setText(oriContent + getString(R.string.fmt_read_zreport, zReportEntry.getTest_index(), zReportEntry.toString()) + "\n");
                m_ScrollView.fullScroll(ScrollView.FOCUS_DOWN);
            } else if (HANDLER_READ_COMPLETE == msg.what) {
                loopCount = -1;
                oriContent = m_ConsoleText.getText().toString();
                m_ConsoleText.setText(oriContent + "Complete...\n");
            } else if (HANDLER_READ_COMPARE == msg.what) {
                oriContent = m_ConsoleText.getText().toString();
                ZReportEntry zReportEntry = (ZReportEntry) msg.obj;

                ZReportEntry oriZReportEntry = cacheZReportEntrys.get(zReportEntry.getTest_index());

                m_ConsoleText.setText(oriContent + getString(R.string.fmt_read_zreport_compare, zReportEntry.getTest_index(),
                        zReportEntry.toString(), oriZReportEntry.toString(), zReportEntry.equals(oriZReportEntry)));
                m_ScrollView.fullScroll(ScrollView.FOCUS_DOWN);

                if (!zReportEntry.equals(oriZReportEntry)) {
                    stop = true;
                }
            } else if (HANDLER_READ_COMPARE_COMPLETE == msg.what) {
                m_ConsoleText.setText(m_ConsoleText.getText().toString() + "Complete...\n");
            } else if (HANDLER_LOOPFUNC_PRINT_CONTENT == msg.what) {
                oriContent = m_ConsoleText.getText().toString();

                String text = (String) msg.obj;

                loopFuncCount++;

                m_ConsoleText.setText(getString(R.string.fmt_loopfunc, oriContent, loopFuncCount, text));

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
    private EditText mEtReadZReportIndex;
    private AlertDialog openingDialog;

    private int loopCount = -1;
    private boolean stop = false;
    private static final int COUNT = 2500;

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

        mEtReadZReportIndex = (EditText) findViewById(R.id.et_read_zreport_index);
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
                        String fmt = getString(R.string.fmt_date, year, month + 1, dayOfMonth);
                        android.util.Log.e(TAG, "[zys-->showDatePickerDialog] fmt:" + fmt);
                        tv.setText(fmt);
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

    boolean EnableUserMode = false;

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

                if (EnableUserMode) {
                    EnableUserMode = false;
                } else {
                    EnableUserMode = true;
                }

                ret = m_FiscalFmemory.SetMode(EnableUserMode);

                OutStr += (ret == FiscalMemory.CMD_OK) ? "SetMode:" + EnableUserMode + ":Success" : "SetMode:" + EnableUserMode + ":Fail";

                OutStr += "\n";
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
                        OutStr += "GetFiscalCode:" + AsciiToString(fis);
                    } else {
                        OutStr += "GetFiscalCode: NULL";
                    }
                    OutStr += "\n";
                    byte[] fisnum = m_FiscalFmemory.GetFiscalNumber();
                    if (fisnum != null) {
                        OutStr += "GetFiscalNum:" + AsciiToString(fisnum);
                    } else {
                        OutStr += "GetFiscalNum: NULL";
                    }

                    OutStr += "\n";

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

                ZReportEntry zReportEntry = createZReportEntry();
                if (zReportEntry == null) {
                    return;
                }

                byte[] zReportData = zReportEntry.getZReportData();

                android.util.Log.e(TAG, "[zys-->] write:" + bytes2BinaryStr(zReportData));

                ret = m_FiscalFmemory.SetEntryData(zReportData);

                OutStr += (ret == FiscalMemory.CMD_OK) ? "Write Z Report Success\n" + zReportEntry.toString() : "Write Z Report Fail";
                OutStr += "\n";
                break;

            case R.id.btn_read_z_report:

                String zreportIndex = mEtReadZReportIndex.getText().toString();
                if (TextUtils.isEmpty(zreportIndex)) {
                    zreportIndex = "0";
                }

                int index = Integer.parseInt(zreportIndex);

                ret = m_FiscalFmemory.SetEntryIndex(index);

                if (ret == FiscalMemory.CMD_OK) {
                    byte[] bytes = m_FiscalFmemory.GetEntryData();

                    android.util.Log.e(TAG, "[zys-->] read:" + bytes2BinaryStr(bytes));

                    ZReportEntry zReportEntry1 = ZReportEntry.parseEntry(bytes);

                    OutStr += "Get index:" + index + " " + zReportEntry1.toString();
                } else {
                    OutStr += "Get index:" + index + " Z Report is Empty.";
                }

                OutStr += "\n";

                break;

            case R.id.tv_date:
                showDatePickerDialog(this, mTvDate, Calendar.getInstance());
                break;

            case R.id.tv_time:
                showTimePickerDialog(this, mTvTime, Calendar.getInstance());
                break;

            case R.id.btn_getfullstatus:

                boolean b = m_FiscalFmemory.GetFullStatus();

                OutStr += b ? "FiscalMemory is full\n" : "FiscalMemory have space\n";

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
                    OutStr += "GetFiscalRevolingAmount : " + new String(revolingAmountBytes);
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

            case R.id.btn_write_loop_z_report:

                loopWriteZReportEntry();

                return;
            case R.id.btn_write_loop_z_report_stop:
                stop = true;
                loopCount = -1;
                break;
            case R.id.btn_read_all_z_report:
                loopReadAllZReportEntry();
                return;

            case R.id.btn_read_range_compared_z_report:
                loopReadZReportEntry2Compared();
                return;

            case R.id.btn_loop_test:

                loopAllFuncTest();

                break;

            default:
                return;
        }

        m_ConsoleText.setText(m_ConsoleText.getText() + OutStr + "\r\n");
        mHandler.post(new Runnable() {
            @Override
            public void run() {
                m_ScrollView.fullScroll(ScrollView.FOCUS_DOWN);
            }
        });
    }

    private void loopReadZReportEntry2Compared() {
        stop = false;

        if (cacheZReportEntrys.size() == 0) {

            Toast.makeText(this, "First Click Read All ZReport Button to Cache ZReport Lists.", Toast.LENGTH_LONG).show();
            return;
        }

        String start2endIndex = mEtStart2EndIndex.getText().toString();
        String[] split = start2endIndex.split(",");
        final int start = Integer.parseInt(split[0]);
        final int end = Integer.parseInt(split[1]);

        if (loopCount == -1) {
            new Thread() {
                @Override
                public void run() {
                    loopCount = start;
                    int count = end + 1;
                    while (loopCount < count && !stop) {

                        int ret = m_FiscalFmemory.SetEntryIndex(loopCount);
                        if (ret == FiscalMemory.CMD_OK) {
                            byte[] bytes = m_FiscalFmemory.GetEntryData();
                            if (bytes != null) {
                                ZReportEntry zReportEntry = ZReportEntry.parseEntry(bytes);
                                zReportEntry.setTest_index(loopCount);
                                Message message = mHandler.obtainMessage();
                                message.what = HANDLER_READ_COMPARE;
                                message.obj = zReportEntry;
                                mHandler.sendMessage(message);
                            }
                        }

                        loopCount++;
                        if (loopCount == count) {
                            mHandler.sendEmptyMessage(HANDLER_READ_COMPARE_COMPLETE);
                        }
                    }

                    loopCount = -1;

                }
            }.start();
        }
    }

    private void loopReadAllZReportEntry() {
        oriContent = null;
        stop = false;
        if (loopCount == -1) {
            new Thread() {
                @Override
                public void run() {

                    loopCount = 0;
                    int count = m_FiscalFmemory.GetNumberOfEntries();
                    cacheZReportEntrys.clear();

                    while (loopCount < count && !stop && loopCount != -1) {
                        int ret = m_FiscalFmemory.SetEntryIndex(loopCount);
                        if (ret == FiscalMemory.CMD_OK) {
                            byte[] bytes = m_FiscalFmemory.GetEntryData();
                            if (bytes != null) {
                                ZReportEntry zReportEntry = ZReportEntry.parseEntry(bytes);
                                cacheZReportEntrys.add(zReportEntry);
//                                zReportEntry.setTest_index(loopCount);
//                                Message message = mHandler.obtainMessage();
//                                message.what = HANDLER_READ_SUCCEED;
//                                message.obj = zReportEntry;
//                                mHandler.sendMessage(message);
                            } else {

                            }
                        } else {
                        }

                        loopCount++;
                        if (loopCount == count) {
                            mHandler.sendEmptyMessage(HANDLER_READ_COMPLETE);
                        }
                    }

                    loopCount = -1;
                }
            }.start();
        }
    }

    private void loopWriteZReportEntry() {
        if (loopCount == -1) {

            stop = false;
            oriContent = null;
            cacheZReportEntrys.clear();

            new Thread() {
                @Override
                public void run() {

                    ZReportEntry zReportEntry = createZReportEntry();

                    int count = m_FiscalFmemory.GetNumberOfEntries();
                    loopCount = count;

                    while (loopCount < COUNT && !stop && loopCount != -1) {
                        zReportEntry = createZReportEntryIncrementTime(zReportEntry);
                        byte[] zReportData = zReportEntry.getZReportData();

                        int ret = m_FiscalFmemory.SetEntryData(zReportData);

                        android.util.Log.e(TAG, "[zys-->] loopCount:" + loopCount + ", ret:" + ret);

                        if (ret == FiscalMemory.CMD_OK) {


                            zReportEntry.setTest_index(loopCount);
                            Message message = mHandler.obtainMessage();
                            message.what = HANDLER_WRITE_SUCCEED;
                            message.obj = zReportEntry;
                            mHandler.sendMessage(message);

                            zReportEntry.setYear(zReportEntry.getYear() + 2000);
                            cacheZReportEntrys.add(zReportEntry);

                        } else {
                            loopCount = -1;
                            mHandler.sendEmptyMessage(HANDLER_WRITE_COMPLETE);
                            break;
                        }
                        loopCount++;
                        if (loopCount == COUNT) {
                            mHandler.sendEmptyMessage(HANDLER_WRITE_COMPLETE);
                        }
                    }

                    loopCount = -1;

                }
            }.start();
        }
    }

    private ZReportEntry createZReportEntryIncrementTime(ZReportEntry z) {
        Calendar c = Calendar.getInstance();
        c.set(Calendar.YEAR, z.getYear());
        c.set(Calendar.MONTH, z.getMonth() - 1);
        c.set(Calendar.DAY_OF_MONTH, z.getDay());
        c.set(Calendar.HOUR_OF_DAY, z.getHour());
        c.set(Calendar.MINUTE, z.getMonth());

        c.add(Calendar.HOUR_OF_DAY, 1);

        int year = c.get(Calendar.YEAR);
        int month = c.get(Calendar.MONTH) + 1;
        int day = c.get(Calendar.DAY_OF_MONTH);
        int hour = c.get(Calendar.HOUR_OF_DAY);
        int minute = c.get(Calendar.MINUTE);

        z.setYear(year);
        z.setMonth(month);
        z.setDay(day);
        z.setHour(hour);
        z.setMinute(minute);

        return z;
    }

    private ZReportEntry createZReportEntry() {
        String totalTax = mEtTotalTax.getText().toString();
        String date = mTvDate.getText().toString();
        String time = mTvTime.getText().toString();

        if (TextUtils.isEmpty(totalTax) || TextUtils.isEmpty(date) || TextUtils.isEmpty(time)) {
            return null;
        }

        String[] split = totalTax.split(",");
        int total = Integer.parseInt(split[0]);
        int tax = Integer.parseInt(split[1]);
        short serialNum = Short.parseShort(split[2]);

        String[] split1 = date.split(",");
        int year = Integer.parseInt(split1[0]);
        int month = Integer.parseInt(split1[1]);
        int day = Short.parseShort(split1[2]);

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

        return zReportEntry;
    }

    private void SetDailySalesTotalSumRange() {
        String start2endIndex = mEtStart2EndIndex.getText().toString();
        if (TextUtils.isEmpty(start2endIndex)) {
            String s1 = mTvDailySalesDateStart.getText().toString();
            android.util.Log.e(TAG, "[zys-->] start:" + s1);
            String[] split1 = s1.split(",");
            String s2 = mTvDailySalesTimeStart.getText().toString();
            android.util.Log.e(TAG, "[zys-->] start:" + s2);
            String[] split2 = s2.split(",");

            int s_year = Integer.parseInt(split1[0]);
            int s_month = Integer.parseInt(split1[1]);
            int s_day = Integer.parseInt(split1[2]);

            int s_hour = Integer.parseInt(split2[0]);
            int s_min = Integer.parseInt(split2[1]);

            Calendar s = Calendar.getInstance();
            s.set(Calendar.YEAR, s_year);
            s.set(Calendar.MONTH, s_month - 1);
            s.set(Calendar.DAY_OF_MONTH, s_day);
            s.set(Calendar.HOUR_OF_DAY, s_hour);
            s.set(Calendar.MINUTE, s_min);

            String s3 = mTvDailySalesDateEnd.getText().toString();
            android.util.Log.e(TAG, "[zys-->] end:" + s3);
            String[] split3 = s3.split(",");
            String s4 = mTvDailySalesTimeEnd.getText().toString();
            android.util.Log.e(TAG, "[zys-->] end:" + s4);
            String[] split4 = s4.split(",");

            int e_year = Integer.parseInt(split3[0]);
            int e_month = Integer.parseInt(split3[1]);
            int e_day = Integer.parseInt(split3[2]);

            int e_hour = Integer.parseInt(split4[0]);
            int e_min = Integer.parseInt(split4[1]);

            Calendar e = Calendar.getInstance();
            e.set(Calendar.YEAR, e_year);
            e.set(Calendar.MONTH, e_month - 1);
            e.set(Calendar.DAY_OF_MONTH, e_day);
            e.set(Calendar.HOUR_OF_DAY, e_hour);
            e.set(Calendar.MINUTE, e_min);

            m_FiscalFmemory.SetDailySalesTotalSumRangeByDateTime(s, e);
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

    //---------------------------------------------------------------------------

    private void loopAllFuncTest() {
        new Thread() {
            @Override
            public void run() {
                loopFuncCount = 0;
                String sendMesg = "Opening...";
                Message msg = mHandler.obtainMessage();
                msg.what = HANDLER_LOOPFUNC_PRINT_CONTENT;
                msg.obj = sendMesg;
                mHandler.sendMessage(msg);

                String tpmStr = "Open FiscalFmemory: ";
                boolean result = m_FiscalFmemory.Open();
                if (result) {
                    tpmStr += "Success";
                } else {
                    tpmStr += "Fail";
                }
                msg = mHandler.obtainMessage();
                msg.what = HANDLER_LOOPFUNC_PRINT_CONTENT;
                msg.obj = tpmStr;
                mHandler.sendMessage(msg);

                if (!result) {
                    return;
                }

                String OutStr = null;

                int ret = m_FiscalFmemory.EraseCard();
                if (ret >= 0) {
                    OutStr = "EraseCard Success";
                } else {
                    OutStr = "EraseCard Fail " + ret;
                }
                msg = mHandler.obtainMessage();
                msg.what = HANDLER_LOOPFUNC_PRINT_CONTENT;
                msg.obj = OutStr;
                mHandler.sendMessage(msg);

                //工厂模式
                m_FiscalFmemory.SetMode(false);
                msg = mHandler.obtainMessage();
                msg.what = HANDLER_LOOPFUNC_PRINT_CONTENT;
                msg.obj = "SetMode:false";
                mHandler.sendMessage(msg);

                //--------------------------------------------

                test();

                //--------------------------------------------

                //用户模式
                m_FiscalFmemory.SetMode(true);
                msg = mHandler.obtainMessage();
                msg.what = HANDLER_LOOPFUNC_PRINT_CONTENT;
                msg.obj = "SetMode:true";
                mHandler.sendMessage(msg);

                test();

            }
        }.start();
    }

    private void test() {

        String OutStr = null;
        loopFuncCount = 0;
        int ret = -1;


        String mGetFirmwareInfo = m_FiscalFmemory.GetFirmwareInfo();
        if (mGetFirmwareInfo != null) {
            OutStr += "FirmwareInfo is:" + mGetFirmwareInfo;
        } else {
            OutStr += "FirmwareInfo Test Fail";
        }
        Message msg = mHandler.obtainMessage();
        msg.what = HANDLER_LOOPFUNC_PRINT_CONTENT;
        msg.obj = OutStr;
        mHandler.sendMessage(msg);

        //--------------------------------------------
        boolean b = m_FiscalFmemory.GetFullStatus();
        OutStr = b ? "FiscalMemory is full" : "FiscalMemory have space";
        msg = mHandler.obtainMessage();
        msg.what = HANDLER_LOOPFUNC_PRINT_CONTENT;
        msg.obj = OutStr;
        mHandler.sendMessage(msg);

        //--------------------------------------------
        int i1 = m_FiscalFmemory.GetFreeEntries();

        int loopCountZ = i1;

        OutStr = "Z Report Free space:" + i1;
        msg = mHandler.obtainMessage();
        msg.what = HANDLER_LOOPFUNC_PRINT_CONTENT;
        msg.obj = OutStr;
        mHandler.sendMessage(msg);

        //--------------------------------------------
        int i = m_FiscalFmemory.GetNumberOfEntries();
        OutStr = "Z Report number:" + i;
        msg = mHandler.obtainMessage();
        msg.what = HANDLER_LOOPFUNC_PRINT_CONTENT;
        msg.obj = OutStr;
        mHandler.sendMessage(msg);

        //--------------------------------------------
        String fiscalCode = "asdfghjklqwerty";
        ret = m_FiscalFmemory.SetFiscalCode(fiscalCode.getBytes());
        OutStr = (ret == FiscalMemory.CMD_OK) ? "SetFiscalCode:Success" : "SetFiscalCode:Fail";
        msg = mHandler.obtainMessage();
        msg.what = HANDLER_LOOPFUNC_PRINT_CONTENT;
        msg.obj = OutStr;
        mHandler.sendMessage(msg);

        //--------------------------------------------
        String fiscalNum = "123456789012345";
        ret = m_FiscalFmemory.SetFiscalNumber(fiscalNum.getBytes());
        OutStr = (ret == FiscalMemory.CMD_OK) ? "SetFiscalNumber:Success" : "SetFiscalNumber:Fail";
        msg = mHandler.obtainMessage();
        msg.what = HANDLER_LOOPFUNC_PRINT_CONTENT;
        msg.obj = OutStr;
        mHandler.sendMessage(msg);

        //--------------------------------------------
        byte[] fis = m_FiscalFmemory.GetFiscalCode();
        if (fis != null) {
            OutStr = "GetFiscalCode:" + new String(fis);
        } else {
            OutStr = "GetFiscalCode: NULL";
        }
        msg = mHandler.obtainMessage();
        msg.what = HANDLER_LOOPFUNC_PRINT_CONTENT;
        msg.obj = OutStr;
        mHandler.sendMessage(msg);

        //--------------------------------------------
        byte[] fisnum = m_FiscalFmemory.GetFiscalNumber();
        if (fisnum != null) {
            OutStr = "GetFiscalNum:" + new String(fisnum);
        } else {
            OutStr = "GetFiscalNum: NULL";
        }
        msg = mHandler.obtainMessage();
        msg.what = HANDLER_LOOPFUNC_PRINT_CONTENT;
        msg.obj = OutStr;
        mHandler.sendMessage(msg);

        //--------------------------------------------
        String revolvingAmountStr = "zxcvbnmasdfghjk";
        ret = m_FiscalFmemory.SetFiscalRevolingAmount(revolvingAmountStr.getBytes());
        OutStr = (ret == FiscalMemory.CMD_OK) ? "SetFiscalRevolingAmount:Success" : "SetFiscalRevolingAmount:Fail";
        msg = mHandler.obtainMessage();
        msg.what = HANDLER_LOOPFUNC_PRINT_CONTENT;
        msg.obj = OutStr;
        mHandler.sendMessage(msg);

        //--------------------------------------------
        byte[] revolingAmountBytes = m_FiscalFmemory.GetFiscalRevolingAmount();
        if (revolingAmountBytes != null) {
            OutStr = "GetFiscalRevolingAmount : " + new String(revolingAmountBytes);
        } else {
            OutStr = "GetFiscalRevolingAmount : Empty";
        }
        msg = mHandler.obtainMessage();
        msg.what = HANDLER_LOOPFUNC_PRINT_CONTENT;
        msg.obj = OutStr;
        mHandler.sendMessage(msg);

        //--------------------------------------------
        msg = mHandler.obtainMessage();
        msg.what = HANDLER_LOOPFUNC_PRINT_CONTENT;
        msg.obj = "Writing " + loopCountZ + " times Z report...";
        mHandler.sendMessage(msg);

        stop = false;
        loopCount = m_FiscalFmemory.GetNumberOfEntries();
        cacheZReportEntrys.clear();

        while (loopCount < loopCountZ && !stop && loopCount != -1) {
            ZReportEntry zReportEntry = createTestReportEntry();
            byte[] zReportData = zReportEntry.getZReportData();
            ret = m_FiscalFmemory.SetEntryData(zReportData);

            android.util.Log.e(TAG, "[zys-->SetEntryData] loopCount:" + loopCount);

            if (ret == FiscalMemory.CMD_OK) {
                cacheZReportEntrys.add(zReportEntry);

                msg = mHandler.obtainMessage();
                msg.what = HANDLER_LOOPFUNC_PRINT_CONTENT;
                msg.obj = "Writing " + loopCount + " times Z report";
                mHandler.sendMessage(msg);
            } else {
                loopCount = -1;
                stop = true;

                msg = mHandler.obtainMessage();
                msg.what = HANDLER_LOOPFUNC_PRINT_CONTENT;
                msg.obj = "Write Z Report Failed.";
                mHandler.sendMessage(msg);
                return;
            }
            loopCount++;
            if (loopCount == loopCountZ) {
                msg = mHandler.obtainMessage();
                msg.what = HANDLER_LOOPFUNC_PRINT_CONTENT;
                msg.obj = "Write Z Report Complete.";
                mHandler.sendMessage(msg);
            }
        }

        //--------------------------------------------

        msg = mHandler.obtainMessage();
        msg.what = HANDLER_LOOPFUNC_PRINT_CONTENT;
        msg.obj = "Comparing " + loopCountZ + " times Z report data...";
        mHandler.sendMessage(msg);

        loopCount = 0;
        stop = false;

        while (loopCount < loopCountZ && !stop) {

            android.util.Log.e(TAG, "[zys-->GetEntryData] loopCount:" + loopCount);

            ret = m_FiscalFmemory.SetEntryIndex(loopCount);
            if (ret == FiscalMemory.CMD_OK) {
                byte[] bytes = m_FiscalFmemory.GetEntryData();
                if (bytes != null) {
                    ZReportEntry zReportEntry = ZReportEntry.parseEntry(bytes);
                    ZReportEntry z = cacheZReportEntrys.get(loopCount);
                    if (!zReportEntry.equals(z)) {
                        msg = mHandler.obtainMessage();
                        msg.what = HANDLER_LOOPFUNC_PRINT_CONTENT;
                        msg.obj = "Compare Failed.";
                        mHandler.sendMessage(msg);
                        return;
                    } else {
                        msg = mHandler.obtainMessage();
                        msg.what = HANDLER_LOOPFUNC_PRINT_CONTENT;
                        msg.obj = "Comparing index:" + loopCount + " the same.";
                        mHandler.sendMessage(msg);
                    }
                } else {
                    msg = mHandler.obtainMessage();
                    msg.what = HANDLER_LOOPFUNC_PRINT_CONTENT;
                    msg.obj = "Compare Failed.";
                    mHandler.sendMessage(msg);
                    return;
                }
            } else {
                msg = mHandler.obtainMessage();
                msg.what = HANDLER_LOOPFUNC_PRINT_CONTENT;
                msg.obj = "Compare Failed.";
                mHandler.sendMessage(msg);
                return;
            }

            loopCount++;
            if (loopCount == loopCountZ) {
                msg = mHandler.obtainMessage();
                msg.what = HANDLER_LOOPFUNC_PRINT_CONTENT;
                msg.obj = "Compare Complete.";
                mHandler.sendMessage(msg);
            }
        }
    }

    private ZReportEntry createLoopZReportEntryIncrement(ZReportEntry z) {
        Calendar c = Calendar.getInstance();
        c.set(Calendar.YEAR, z.getYear());
        c.set(Calendar.MONTH, z.getMonth() - 1);
        c.set(Calendar.DAY_OF_MONTH, z.getDay());
        c.set(Calendar.HOUR_OF_DAY, z.getHour());
        c.set(Calendar.MINUTE, z.getMonth());

        c.add(Calendar.HOUR_OF_DAY, 1);

        int year = c.get(Calendar.YEAR);
        int month = c.get(Calendar.MONTH) + 1;
        int day = c.get(Calendar.DAY_OF_MONTH);
        int hour = c.get(Calendar.HOUR_OF_DAY);
        int minute = c.get(Calendar.MINUTE);

        z.setYear(year);
        z.setMonth(month);
        z.setDay(day);
        z.setHour(hour);
        z.setMinute(minute);

        z.setSales_tax(z.getSales_tax() + loopCount);
        z.setSales_total(z.getSales_total() + loopCount);

        return z;
    }

    private ZReportEntry createTestReportEntry() {

        int total = loopCount + 1;
        int tax = loopCount + 1;
        short serialNum = 123;

        int year = 2018;
        int month = 1;
        int day = 1;

        int hour = 0;
        int minute = 0;

        ZReportEntry zReportEntry = new ZReportEntry();
        zReportEntry.setSales_total(total);
        zReportEntry.setSales_tax(tax);
        zReportEntry.setYear(year);
        zReportEntry.setMonth(month);
        zReportEntry.setDay(day);
        zReportEntry.setHour(hour);
        zReportEntry.setMinute(minute);
        zReportEntry.setSerial_number(serialNum);

        return zReportEntry;
    }

}
