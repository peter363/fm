#include<iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "SerialMonitor.h"
#include "ProtocolParser.h"
#include "OtpMonitor.h"
#include "Configuration.h"

using namespace std;

uint8_t const TestCommand111[] =
{
    0x02, 0x00,
    0x00, 0x0A,
    0xFF, 0xF5,
    0x0D, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

void DumpHex(uint8_t const *buf, uint32_t len)
{
    int i;

    for(i=0; i<len; i++)
    {
        printf("%02x ", buf[i]);
    }
    printf("\n");
}

void FmTest(uint8_t const * buf)
{
    str_fmInfo info;
    OtpMonitor om;
    int32_t ret;
    uint8_t testbuf[2000];
    FM_CONTEXT_t fm_ctx;
           
    om.Open(&fm_ctx);
    info = om.GetFmInfo();
    om.EraseAll();

    //write test
    for(uint32_t addr=0; addr<info.fm_size; addr+=info.fm_single_write_size)
    {
        memset(testbuf, 0, sizeof(testbuf));
        for(int32_t j=0; j<info.fm_single_write_size; j+=4)
        {
            *((uint32_t *)&testbuf[j]) = addr + j; 
        }

        printf("addr %x, test %u wrsize %x\n", addr, *((uint32_t *)&testbuf[0]), info.fm_single_write_size);
        ret = om.WriteData(addr, (char *)testbuf, info.fm_single_write_size); 
        if(ret != info.fm_single_write_size)
        {
            printf("[TEST] Write Error at %x\n", addr);
            //om.Close();
            //return;
        }
    }
    om.Close();

    om.Open(&fm_ctx);
    info = om.GetFmInfo();
    //read test
    for(uint32_t addr=0; addr<info.fm_size; addr+=info.fm_single_write_size)
    {
        memset(testbuf, 0, sizeof(testbuf));

        ret = om.ReadData(addr, (char *)testbuf, info.fm_single_write_size); 
        if(ret != info.fm_single_write_size)
        {
            printf("[TEST] Read Error at %x\n", addr);
            //om.Close();
            //return;
        }
        for(int32_t j=0; j<info.fm_single_write_size; j+=4)
        {
            if(*((uint32_t *)&testbuf[j]) != (addr + j))
            {
                printf("[TEST] Read Value Error Excetp %x(%x + %x), It's %x\n", addr + j, addr, j, *((uint32_t *)&testbuf[j]));
            }
        }
    }

    om.Close();
}

#define FISCAL_CODE "1234567890"

int main(int argc, char *argv[])
{
    uint8_t buf[2000];
    uint8_t wr_buf[] = { 0xab, 0x55 };
    str_fmInfo info;
    int ret;
    int para;
    uint32_t addr;    
    OtpMonitor om;
    FM_CONTEXT_t fm_ctx;

    const char *optstring = "c:piretda:w:"; // 有三个选项-abc，其中c选项后有冒号，所以后面必须有参数
    while ((para = getopt(argc, argv, optstring)) != -1) {
        switch (para) {
            case 'c':
            {
                printf("opt is Configuration, oprarg is: %s\n", optarg);                
                Configuration cg(&fm_ctx);
                if(!strcmp(optarg, "wrfis"))
                {                    
                    cg.SetFiscalCode((const uint8_t *)FISCAL_CODE, strlen(FISCAL_CODE));
                }else if(!strcmp(optarg, "rdfis"))
                {
                    uint8_t len = 100;
                    cg.GetFiscalCode(buf, len);
                    buf[len] = 0;
                    printf("FiscalCode is %s\n", buf);
                }
            }
                break;
            case 'a':
                sscanf(optarg, "%x", &addr);
                printf("optarg(%s) addr is: %x\n", optarg, addr);
                break;
            case 't':
                printf("opt is test, addr is :%x\n", addr);
                //strcpy((char *)buf, optarg);  
                FmTest(buf);   
                break;                
            case 'w':
                printf("opt is write, oprarg is: %s addr is :%x\n", optarg, addr);
                strcpy((char *)buf, optarg);                
                om.Open(&fm_ctx);
                om.WriteData(addr, (char *)buf, strlen((char const *)buf));
                om.Close();
                break;
            case 'r':
                printf("opt is read, oprarg is: %s addr is :%x\n", optarg, addr);
                om.Open(&fm_ctx);
                ret = om.ReadData(addr, buf, sizeof(buf));
                printf("ret = %d\n", ret);
                om.Close();
                if(ret >= 0)
                    DumpHex(buf, ret);
                break;
            case 'e':
                printf("opt is erase, oprarg is: %s\n", optarg);
                om.Open(&fm_ctx);
                om.EraseAll();
                om.Close();
                break;
            case 'd':
                printf("opt is dump, oprarg is: %s\n", optarg);
                om.Open(&fm_ctx);
                om.DumpMap();
                om.Close();
                break;
            case 'i':
                printf("opt is dump, oprarg is: %s\n", optarg);
                break;                
            case 'p':
            {
                ProtocolParser pp;
                pp.Open(&fm_ctx);
                ret = pp.ReadData(0, buf ,100);
                cout << "ret = " << ret << endl;
                if(ret > 0) DumpHex(buf, ret);

                ret = pp.WriteData(5, wr_buf , sizeof(wr_buf));
                cout << "ret = " << ret << endl;
                if(ret > 0) DumpHex(buf, ret);

                ret = pp.ReadData(0, buf ,100);
                cout << "ret = " << ret << endl;
                if(ret > 0) DumpHex(buf, ret);


                ret = pp.GetFmInfo(&info);
                pp.Close();
                cout << "ret = " << ret << endl;
                //DumpHex(buf, ret);
            }

                break;   
            case '?':
                printf("error optopt: %c\n", optopt);
                printf("error opterr: %d\n", opterr);
                break;
        }
    }

    //om.DumpMap();

    
    return 0;
}


