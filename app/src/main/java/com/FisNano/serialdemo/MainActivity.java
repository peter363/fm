package com.FisNano.serialdemo;

import android.os.Bundle;

import com.FisNano.FiscalMemory;
import com.FisNano.FiscalMemory;
import com.google.android.material.floatingactionbutton.FloatingActionButton;
import com.google.android.material.snackbar.Snackbar;

import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.Toolbar;

import android.util.Log;
import android.view.View;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.Button;
import android.widget.ScrollView;
import android.widget.TextView;
import android.content.Context;

import java.io.UnsupportedEncodingException;
//import android.hardware.SerialManager;


public class MainActivity extends AppCompatActivity {
    static String TAG = "FiscalMem";

    Button m_OpenBtn, m_CloseBtn;
    TextView m_ConsoleText;
    ScrollView m_ScrollView;
    FiscalMemory m_FiscalFmemory;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
//        Toolbar toolbar = findViewById(R.id.toolbar);
//        setSupportActionBar(toolbar);

        m_ConsoleText = findViewById(R.id.LogText);
        m_OpenBtn = findViewById(R.id.btn_open);
        m_CloseBtn = findViewById(R.id.btn_close);
        m_ScrollView = findViewById(R.id.up_scrollview);

//        FloatingActionButton fab = findViewById(R.id.fab);
//        fab.setOnClickListener(new View.OnClickListener() {
//            @Override
//            public void onClick(View view) {
//                Snackbar.make(view, "Replace with your own action", Snackbar.LENGTH_LONG)
//                        .setAction("Action", null).show();
//            }
//        });

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
     * @param value
     * @return
     */
    public static String AsciiToString(byte[] value) throws UnsupportedEncodingException {
        return new String(value,"ISO-8859-1");
    }

    public void onClick(View v){
        int ret;
        String OutStr = "";
        Log.d(TAG, "V.id = " + v.getId());
        switch(v.getId())
        {
            case R.id.btn_open:
                m_ConsoleText.setText(m_ConsoleText.getText() + "Opening..." + "\r\n");
                m_ScrollView.fullScroll(ScrollView.FOCUS_DOWN);
                OutStr = "Open FiscalFmemory: ";
                boolean result = m_FiscalFmemory.Open();
                if(result)
                {
                    OutStr += "Success";
                }else
                {
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
                        .SetFiscalCode(new byte[]{ 'F', 'i','s','c','a','l','C','o','d','e','1','2','3','4','5' });
                OutStr += (ret == FiscalMemory.CMD_OK) ? "Success" : "Fail";

                ret = m_FiscalFmemory.SetFiscalNum(new byte[]{ 'F', 'i','s','c','a','l','N','u','m','3','3','2','3','4','5' });
                OutStr += (ret == FiscalMemory.CMD_OK) ? "Success" : "Fail";

                break;

            case R.id.btn_read:
                try
                {
                    String mGetFirmwareInfo = m_FiscalFmemory.GetFirmwareInfo();
                    byte[] fis = m_FiscalFmemory.GetFiscalCode();
                    if(fis != null)
                    {
                        OutStr += "GetFiscalCode " + AsciiToString(fis) + "\r\n";
                        Log.d(TAG, "GetFiscalCode " + AsciiToString(fis));
                    }
                    byte[] fisnum = m_FiscalFmemory.GetFiscalNum();
                    if(fisnum != null)
                    {
                        OutStr += "GetFiscalNum " + AsciiToString(fisnum) + "\r\n";
                        Log.d(TAG, "GetFiscalNum " + AsciiToString(fisnum));
                    }
                    if(mGetFirmwareInfo != null)
                    {
                        OutStr += "FirmwareInfo is " + mGetFirmwareInfo + "\r\n";
                        Log.d(TAG, "FirmwareInfo is " + mGetFirmwareInfo);
                    }else
                    {
                        OutStr += "Test Fail\r\n";
                        Log.e(TAG, "Test Fail");
                    }
                }catch (IllegalArgumentException | UnsupportedEncodingException e)
                {
                    e.printStackTrace();
                }
                break;

            case R.id.btn_erase:
                OutStr = "Erase FiscalFmemory: ";
                ret = m_FiscalFmemory.EraseCard();
                if(ret >= 0)
                {
                    OutStr += "Success";
                }else
                {
                    OutStr += "Fail " + ret;
                }
                break;

            case R.id.btn_reset:
                OutStr = "SoftwareReset FiscalFmemory: ";
                ret = m_FiscalFmemory.SoftwareReset();
                OutStr += (ret == FiscalMemory.CMD_OK) ? "Success" : "Fail";

            default:return;
        }

        m_ConsoleText.setText(m_ConsoleText.getText() + OutStr + "\r\n");
        m_ScrollView.fullScroll(ScrollView.FOCUS_DOWN);
    }
}
