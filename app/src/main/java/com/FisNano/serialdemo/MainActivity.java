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

    TextView m_ConsoleText;
    ScrollView m_ScrollView;
    FiscalMemory m_FiscalFmemory;
    String oriContent = null;

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

                if (TextUtils.isEmpty(oriContent)) {
                    oriContent = m_ConsoleText.getText().toString();
                }

                int count = (int) msg.obj;

                m_ConsoleText.setText(oriContent + getString(R.string.write_zreport_test, count));
            } else if (HANDLER_WRITE_COMPLETE == msg.what) {
                loopCount = -1;
                oriContent = m_ConsoleText.getText().toString();
                m_ConsoleText.setText(oriContent + "Complete " + getString(R.string.write_zreport_test, loopCount) + "\n");
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

                break;
            case R.id.btn_write_loop_z_report_stop:
                stop = true;
                break;
            case R.id.btn_read_all_z_report:
                loopReadAllZReportEntry();
                break;

            case R.id.btn_read_range_compared_z_report:
                loopReadZReportEntry2Compared();
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

                    while (loopCount < count && !stop) {

                        int ret = m_FiscalFmemory.SetEntryIndex(loopCount);

                        if (ret == FiscalMemory.CMD_OK) {
                            byte[] bytes = m_FiscalFmemory.GetEntryData();
                            if (bytes != null) {
                                ZReportEntry zReportEntry = ZReportEntry.parseEntry(bytes);

                                cacheZReportEntrys.add(zReportEntry);

                                zReportEntry.setTest_index(loopCount);
                                Message message = mHandler.obtainMessage();
                                message.what = HANDLER_READ_SUCCEED;
                                message.obj = zReportEntry;
                                mHandler.sendMessage(message);
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
            final ZReportEntry zReportEntry = createZReportEntry();
            if (zReportEntry == null) {
                return;
            }
            stop = false;
            oriContent = null;

            new Thread() {
                @Override
                public void run() {

                    int count = m_FiscalFmemory.GetNumberOfEntries();
                    loopCount = count;

                    while (loopCount < COUNT && !stop) {
                        byte[] zReportData = zReportEntry.getZReportData();

                        int ret = m_FiscalFmemory.SetEntryData(zReportData);
                        if (ret == FiscalMemory.CMD_OK) {

                            cacheZReportEntrys.add(zReportEntry);

                            Message message = mHandler.obtainMessage();
                            message.what = HANDLER_WRITE_SUCCEED;
                            message.obj = loopCount;
                            mHandler.sendMessage(message);
                        } else {
                            mHandler.sendEmptyMessage(HANDLER_WRITE_COMPLETE);
                            return;
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
        c.add(Calendar.MINUTE, 30);

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
            String[] split1 = s1.split(",");
            String s2 = mTvDailySalesTimeStart.getText().toString();
            String[] split2 = s2.split(",");

            int s_year = Integer.parseInt(split1[0]);
            int s_month = Integer.parseInt(split1[1]);
            int s_day = Integer.parseInt(split1[2]);

            int s_hour = Integer.parseInt(split2[0]);
            int s_min = Integer.parseInt(split2[1]);

            Calendar c = Calendar.getInstance();
            c.set(Calendar.YEAR, s_year);
            c.set(Calendar.MONTH, s_month - 1);
            c.set(Calendar.DAY_OF_MONTH, s_day);
            c.set(Calendar.HOUR_OF_DAY, s_hour);
            c.set(Calendar.MINUTE, s_min);
            Date startDate = c.getTime();

            String s3 = mTvDailySalesDateEnd.getText().toString();
            String[] split3 = s3.split(",");
            String s4 = mTvDailySalesTimeEnd.getText().toString();
            String[] split4 = s4.split(",");

            int e_year = Integer.parseInt(split3[0]);
            int e_month = Integer.parseInt(split3[1]);
            int e_day = Integer.parseInt(split3[2]);

            int e_hour = Integer.parseInt(split4[0]);
            int e_min = Integer.parseInt(split4[1]);

            c = Calendar.getInstance();
            c.set(Calendar.YEAR, e_year);
            c.set(Calendar.MONTH, e_month - 1);
            c.set(Calendar.DAY_OF_MONTH, e_day);
            c.set(Calendar.HOUR_OF_DAY, e_hour);
            c.set(Calendar.MINUTE, e_min);
            Date endDate = c.getTime();

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
