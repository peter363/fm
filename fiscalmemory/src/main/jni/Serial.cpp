//
// Created by Administrator on 2020/2/22 0022.
//
#define TAG "SERIAL"

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <jni.h>
#include <string.h>
#include <termios.h>
#include<sys/time.h>
#include <errno.h>
#include "Serial.h"
#include "dbg_log.h"

#if 0
void test
{
//实例化Test类
    jclass testclass = (*env)->FindClass(env, "android/hardware/SerialManager");
    //构造函数的方法名为<init>
    jmethodID testcontruct = (*env)->GetMethodID(env, testclass, "<init>", "()V");
    //根据构造函数实例化对象
    jobject testobject = (*env)->NewObject(env, testclass, testcontruct);

    //调用成员方法，需使用jobject对象
    jmethodID test = (*env)->GetMethodID(env, testclass, "test", "(I)V");
    (*env)->CallVoidMethod(env, testobject, test, 1);

    //调用静态方法
    jmethodID testStatic = (*env)->GetStaticMethodID(env, testclass, "testStatic", "(Ljava/lang/String;)V");
    //创建字符串，不能在CallStaticVoidMethod中直接使用"hello world!"，会报错的
    jstring str = (*env)->NewStringUTF(env, "hello world!");
    //调用静态方法使用的是jclass，而不是jobject
    (*env)->CallStaticVoidMethod(env, testclass, testStatic, str);

    //实例化InnerClass子类
    jclass innerclass = (*env)->FindClass(env, "com/lb6905/jnidemo/TestClass$InnerClass");
    jmethodID innercontruct = (*env)->GetMethodID(env, innerclass, "<init>", "()V");
    jobject innerobject = (*env)->NewObject(env, innerclass, innercontruct);

    //调用子类的成员方法
    jmethodID setInt = (*env)->GetMethodID(env, innerclass, "setInt", "(I)V");
    (*env)->CallVoidMethod(env, innerobject, setInt, 2);

}
#endif

#define SERIAL_DEVICE   "/dev/ttyHSL0"

int32_t Serial::SerialDirect_Open()
{
    int32_t fd;

    if(m_Serialfd > 0)
    {
        LOGI("Serial had opened\n");
        return m_Serialfd;
    }

    fd = open(SERIAL_DEVICE, O_RDWR);
    if(fd < 0)
    {
        LOGE("Open Serial Port Error %d\n", fd);
    }

    struct termios tio;
    if (tcgetattr(fd, &tio))
        memset(&tio, 0, sizeof(tio));

    tio.c_cflag =  B115200 | CS8 | CLOCAL | CREAD;
    // Disable output processing, including messing with end-of-line characters.
    tio.c_oflag &= ~OPOST;
    tio.c_iflag = IGNPAR;
    tio.c_lflag = 0; /* turn of CANON, ECHO*, etc */
    /* no timeout but request at least one character per read */
    tio.c_cc[VTIME] = 5;
    tio.c_cc[VMIN] = 0;
    tcsetattr(fd, TCSANOW, &tio);
    tcflush(fd, TCIFLUSH);

    LOGD("Success Open Serial\n");
    m_Serialfd = fd;
    return fd;
}

void DumpHex(uint8_t const *buf, uint32_t len)
{
    int i;

    for(i=0; i<len; i++)
    {
        LOGD("%02x ", buf[i]);
    }
    LOGD("\n");
}

int32_t Serial::SerialDirect_Read(void *buf, uint32_t rd_len)
{
    int32_t ret;
//    LOGD("%s %u\n", __FUNCTION__, rd_len);
    if(m_Serialfd < 0)
    {
        LOGE("Serial Port is not Open!!");
        return -EIO;
    }

    ret = read(m_Serialfd, buf, rd_len);
//    LOGD("read ret = %d", ret);
//    DumpHex(static_cast<const uint8_t *>(buf), ret);
    if(ret < 0)
    {
        LOGE("Read Serial Port Error %d\n", ret);
    }
    return ret;
}

int32_t Serial::SerialDirect_Write(void const *buf, uint32_t wr_len)
{
    int32_t ret;
//    LOGD("%s %u\n", __FUNCTION__, wr_len);
    if(m_Serialfd < 0)
    {
        LOGE("Serial Port is not Open!!");
        return -EIO;
    }
    ret = write(m_Serialfd, buf, wr_len);
    if(ret < 0)
    {
        LOGE("Write Serial Port Error %d\n", ret);
    }
    return ret;
}

void Serial::SerialDirect_Close()
{
    close(m_Serialfd);
    m_Serialfd = -1;
}

int32_t Serial::Open(FM_CONTEXT_t const *fm_ctx)
{
    JNIEnv * env = fm_ctx->env;

    m_fm_ctx = fm_ctx;
    SerialDirect_Open();
//    LOGD("%s env = %p", __FUNCTION__, env);
//    //1.找到Java中要调用方法所在的类
//    //参数解释：
//    //第一个参数：这个直接填传递过来的env即可
//    //第二个参数：方法所在类的全路径
//    jclass jclazz = env->FindClass("android/hardware/SerialManager");
//    //判断是否找到
//    if (jclazz == nullptr) {
//        //找不到则返回
//        LOGE("%s Can't not found SerialManager", __FUNCTION__);
//        return -EIO;
//    }
//
//    jmethodID method = env->GetMethodID(jclazz, "getSerialPorts",
//                                        "()[Ljava/lang/String;");
//    jobjectArray m_StrArray = static_cast<jobjectArray>(env->CallObjectMethod(jclazz, method));
//    jstring port = static_cast<jstring>(env->GetObjectArrayElement(m_StrArray, 0));
//    //2.获取到类中对应的方法
//    //参数解释：
//    //第一个参数：这个直接填传递过来的env即可
//    //第二个参数：第一步产生的jclass
//    //第三个参数：Java中要调用的方法的名称
//    //第四个参数：Java中方法的签名
//    jmethodID method = env->GetMethodID(jclazz, "openSerialPort",
//                                        "(Ljava/lang/String;I)Landroid/hardware/SerialPort;");
//    if (method == 0) {
//        //找不到方法则返回
//        LOGE("%s Can't not found SerialManager method", __FUNCTION__);
//        return -EIO;
//    }
//    //3.调用方法
//    //第一个参数：这个直接填传递过来的env即可
//    //第二个参数：这个直接填传递过来的jobj即可
//    //第三个参数：上一步产生的jmethodID
//    //第四个参数：直接填写传递过来的jstr即可，当然也可以通过NewStringUTF自己创建一个
//    env->NewByteArray()
//    env->CallVoidMethod(jclazz, method, jbyteArray, 115200);

    return 0;
}

void Serial::Close()
{
    SerialDirect_Close();
//    env)->DeleteLocalRef(env,clazz);
}

int32_t Serial::SendData(void const *dat, uint32_t len)
{
    SerialDirect_Write(dat, len);
    return 0;
}

int32_t Serial::ReceiveData(void *recv_data, uint32_t len, uint32_t time_out)
{
    uint32_t recv_len = 0;
    uint32_t ret;
    struct timeval tv, t_tmp;

    gettimeofday(&tv, nullptr);
    t_tmp.tv_usec = time_out * 1000;
    t_tmp.tv_sec = 0;
    timeradd(&tv, &t_tmp, &tv);

    while(recv_len < len)
    {
        ret = SerialDirect_Read(&((uint8_t *)recv_data)[recv_len], len - recv_len);
        gettimeofday(&t_tmp, nullptr);
        if((timercmp(&t_tmp, &tv, >) || (ret <= 0)))
        {
            return recv_len;
        }
        recv_len += ret;
    }

    return recv_len;
}

