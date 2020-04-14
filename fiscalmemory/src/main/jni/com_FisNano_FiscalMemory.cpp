// log标签
#define  TAG    "FiscalMem"
/* Header for class com_FisNano_FiscalMemory */
#include "com_FisNano_FiscalMemory.h"
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cassert>
#include <unistd.h>
#include "dbg_log.h"
#include "FlashManager.h"

// 定义info信息
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,TAG,__VA_ARGS__)

// 定义debug信息
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)

// 定义error信息
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)

static FlashManager * m_FiscalMem = nullptr;

bool CheckInit(JNIEnv * env)
{
    jclass newExcCls;

    if((m_FiscalMem != nullptr) && m_FiscalMem->IsInited())
    {
        return true;
    }

    newExcCls = env->FindClass("java/lang/IllegalArgumentException");
    assert(newExcCls != NULL);
    env->ThrowNew(newExcCls, "***** FiscalMemory Not Init *****");

    return false;
}

/*
 * Class:     com_FisNano_FiscalMemory
 * Method:    OpenDevice
 * Signature: ()I
 */
extern "C" JNIEXPORT jint JNICALL Java_com_FisNano_FiscalMemory_OpenDevice
  (JNIEnv * env, jobject self)
{
    LOGD("%s", __FUNCTION__);
    FM_CONTEXT_t fm_ctx;
    fm_ctx.env = env;
    m_FiscalMem = new FlashManager();
    if(m_FiscalMem == nullptr)
    {
        LOGE("%s new FiscalMem Error", __FUNCTION__);
        return FlashManager::CMD_HARDWARE_FAULT;
    }
    if(m_FiscalMem->Init(&fm_ctx) < 0)
    {
        LOGE("%s FiscalMem Init Error", __FUNCTION__);
        return FlashManager::CMD_HARDWARE_FAULT;
    }

    return FlashManager::CMD_OK;
}

/*
 * Class:     com_FisNano_FiscalMemory
 * Method:    CloseDevie
 * Signature: ()Z
 */
extern "C" JNIEXPORT jboolean JNICALL Java_com_FisNano_FiscalMemory_CloseDevie
  (JNIEnv * env, jobject self)
{
    delete m_FiscalMem;
    m_FiscalMem = nullptr;
    LOGD("%s", __FUNCTION__);
    return FlashManager::CMD_OK;
}

/*
 * Class:     com_FisNano_FiscalMemory
 * Method:    GetFiscalCode
 * Signature: ()[B
 */
extern "C" JNIEXPORT jbyteArray JNICALL Java_com_FisNano_FiscalMemory_GetFiscalCode
  (JNIEnv * env, jobject self)
{
    jbyte m_FiscalCode[40];
    uint8_t len = FlashManager::FISCAL_CODE_MAX_SIZE;

    if(!CheckInit(env)) { return nullptr; }

    int32_t ret = m_FiscalMem->GetFiscalCode((uint8_t *)m_FiscalCode, len);
    if(ret != FlashManager::CMD_OK)
    {
        LOGE("GetFiscal Code Error");
        return nullptr;
    }

    //test code
//    strcpy(reinterpret_cast<char *>(m_FiscalCode), "TestFiscalCode");
//    len = strlen(reinterpret_cast<const char *>(m_FiscalCode));

    jbyteArray mByteArray = env->NewByteArray(len);

    env->SetByteArrayRegion(mByteArray, 0, len, m_FiscalCode);
    return mByteArray;
}
/*
 * Class:     com_FisNano_FiscalMemory
 * Method:    SetFiscalCode
 * Signature: ([B)I
 */
extern "C" JNIEXPORT jint JNICALL Java_com_FisNano_FiscalMemory_SetFiscalCode
  (JNIEnv * env, jobject self, jbyteArray FiscalCode)
{
    if(!CheckInit(env)) { return FlashManager::CMD_HARDWARE_FAULT; }
    jint len = env->GetArrayLength(FiscalCode);
    jbyte* data = env->GetByteArrayElements(FiscalCode, NULL);

    LOGD("%s  %d len %d\n", __FUNCTION__, __LINE__, len);
    if (data != nullptr) {
        LOGE("%s data = %*.*s len %d\n", __FUNCTION__,  len, len, (char *)data, len);
        m_FiscalMem->SetFiscalCode((const uint8_t *)data, len);
        env->ReleaseByteArrayElements(FiscalCode, data, JNI_ABORT);
        return FlashManager::CMD_OK;
    }

    return FlashManager::CMD_ARGUMENT_INVALID;
}
/*
 * Class:     com_FisNano_FiscalMemory
 * Method:    GetFiscalNum
 * Signature: ()[B
 */
extern "C" JNIEXPORT jbyteArray JNICALL Java_com_FisNano_FiscalMemory_GetFiscalNumber(JNIEnv * env, jobject self)
{
    if(!CheckInit(env)) { return nullptr; }

    jbyte m_Fiscal[100];
    uint8_t len = FlashManager::FISCAL_NUMBER_MAX_SIZE;

    int32_t ret = m_FiscalMem->GetFiscalNumber((uint8_t *)m_Fiscal, len);
    if(ret != FlashManager::CMD_OK)
    {
        LOGE("GetFiscal Code Error");
        return nullptr;
    }

    //test code
//    strcpy(reinterpret_cast<char *>(m_FiscalCode), "TestFiscalCode");
//    len = strlen(reinterpret_cast<const char *>(m_FiscalCode));

    jbyteArray mByteArray = env->NewByteArray(len);
    env->SetByteArrayRegion(mByteArray, 0, len, m_Fiscal);
    return mByteArray;
}

/*
 * Class:     com_FisNano_FiscalMemory
 * Method:    SetFiscalNum
 * Signature: ([B)I
 */
extern "C" JNIEXPORT jint JNICALL Java_com_FisNano_FiscalMemory_SetFiscalNumber(JNIEnv *env, jobject thiz, jbyteArray fiscal_num)
{
    if(!CheckInit(env)) { return FlashManager::CMD_HARDWARE_FAULT; }
    jint len = env->GetArrayLength(fiscal_num);
    jbyte* data = env->GetByteArrayElements(fiscal_num, NULL);
    LOGD("%s  %d len %d\n", __FUNCTION__, __LINE__, len);
    if (data != NULL) {
        LOGE("%s data = %*.*s len %d\n", __FUNCTION__,  len, len, (char *)data, len);
        m_FiscalMem->SetFiscalNumber((const uint8_t *)data, len);
        env->ReleaseByteArrayElements(fiscal_num, data, JNI_ABORT);
        return FlashManager::CMD_OK;
    }

    return FlashManager::CMD_ARGUMENT_INVALID;
}
/*
 * Class:     com_FisNano_FiscalMemory
 * Method:    SetEntryData
 * Signature: ()I
 */
extern "C" JNIEXPORT jint JNICALL
Java_com_FisNano_FiscalMemory_SetEntryData(JNIEnv *env, jobject thiz, jbyteArray entry_data) {
    if(!CheckInit(env)) { return FlashManager::CMD_HARDWARE_FAULT; }
    jint len = env->GetArrayLength(entry_data);
    jbyte* data = env->GetByteArrayElements(entry_data, NULL);
    LOGD("%s  %d len %d\n", __FUNCTION__, __LINE__, len);
    if (data != NULL) {
        LOGE("%s data = %*.*s len %d\n", __FUNCTION__,  len, len, (char *)data, len);
        m_FiscalMem->SetEntry((const uint8_t *)data, len);
        env->ReleaseByteArrayElements(entry_data, data, JNI_ABORT);
        return FlashManager::CMD_OK;
    }

    return FlashManager::CMD_ARGUMENT_INVALID;
}

/*
 * Class:     com_FisNano_FiscalMemory
 * Method:    GetNumberOfEntries
 * Signature: ()I
 */
extern "C" JNIEXPORT jint JNICALL Java_com_FisNano_FiscalMemory_GetNumberOfEntries
  (JNIEnv * env, jobject self)
{
    if(!CheckInit(env)) { return FlashManager::CMD_HARDWARE_FAULT; }
    return m_FiscalMem->GetNumberOfEntries();
}

/*
 * Class:     com_FisNano_FiscalMemory
 * Method:    SetEntryIndex
 * Signature: (I)I
 */
extern "C" JNIEXPORT jint JNICALL Java_com_FisNano_FiscalMemory_SetEntryIndex
  (JNIEnv * env, jobject self, jint Index)
{
    if(!CheckInit(env)) { return FlashManager::CMD_HARDWARE_FAULT; }
    jint ret = m_FiscalMem->SetEntryNumber((uint32_t)Index);
    return ret;
}

/*
 * Class:     com_FisNano_FiscalMemory
 * Method:    GetEntryData
 * Signature: ()[B
 */
extern "C" JNIEXPORT jbyteArray JNICALL Java_com_FisNano_FiscalMemory_GetEntryData
  (JNIEnv * env, jobject self, jint index)
{

    uint8_t rd_buf[FlashManager::ENTRY_SIZE + 4];
    uint8_t get_len = FlashManager::ENTRY_SIZE;

    if(!CheckInit(env)) { return nullptr; }
    //
    jint ret = m_FiscalMem->GetEntry(rd_buf, get_len);
    if(ret == FlashManager::CMD_OK && get_len == FlashManager::ENTRY_SIZE)
    {
        jbyteArray mByteArray = env->NewByteArray(FlashManager::ENTRY_SIZE);
        env->SetByteArrayRegion(mByteArray, 0, FlashManager::ENTRY_SIZE,
                                reinterpret_cast<const jbyte *>(rd_buf));
        return mByteArray;
    }

    return nullptr;
}

/*
 * Class:     com_FisNano_FiscalMemory
 * Method:    GetFreeEntries
 * Signature: ()I
 */
extern "C" JNIEXPORT jint JNICALL Java_com_FisNano_FiscalMemory_GetFreeEntries
  (JNIEnv * env, jobject self)
{
    if(!CheckInit(env)) { return FlashManager::CMD_HARDWARE_FAULT; }
    return m_FiscalMem->GetEntrySpace();
}

/*
 * Class:     com_FisNano_FiscalMemory
 * Method:    ClearCompleteCard
 * Signature: ()I
 */
extern "C" JNIEXPORT jint JNICALL Java_com_FisNano_FiscalMemory_ClearCompleteCard
  (JNIEnv * env, jobject self)
{
    if(!CheckInit(env)) { return FlashManager::CMD_HARDWARE_FAULT; }
    m_FiscalMem->ClearCompleteCard();
    return FlashManager::CMD_OK;
}

/*
 * Class:     com_FisNano_FiscalMemory
 * Method:    SoftwareReset
 * Signature: ()I
 */
extern "C" JNIEXPORT jint JNICALL Java_com_FisNano_FiscalMemory_SoftwareReset
  (JNIEnv * env, jobject self)
{
    if(!CheckInit(env)) { return FlashManager::CMD_HARDWARE_FAULT; }
    return FlashManager::CMD_OK;
}

/*
 * Class:     com_FisNano_FiscalMemory
 * Method:    SetFiscalRevolingAmount
 * Signature: ()I
 */
extern "C" JNIEXPORT jint JNICALL
Java_com_FisNano_FiscalMemory_SetFiscalRevolingAmount(JNIEnv *env, jobject thiz,
                                                      jbyteArray revoling_amount)
{
    if(!CheckInit(env)) { return FlashManager::CMD_HARDWARE_FAULT; }
    jint len = env->GetArrayLength(revoling_amount);
    jbyte* data = env->GetByteArrayElements(revoling_amount, NULL);
    LOGD("%s  %d len %d\n", __FUNCTION__, __LINE__, len);
    if (data != NULL) {
        LOGE("%s data = %*.*s len %d\n", __FUNCTION__,  len, len, (char *)data, len);
        m_FiscalMem->SetFiscalRevolvingAmount((const uint8_t *)data, len);
        env->ReleaseByteArrayElements(revoling_amount, data, JNI_ABORT);
        return FlashManager::CMD_OK;
    }

    return FlashManager::CMD_ARGUMENT_INVALID;
}

/*
 * Class:     com_FisNano_FiscalMemory
 * Method:    GetFiscalRevolingAmount
 * Signature: ()I
 */
extern "C"
JNIEXPORT jbyteArray JNICALL
Java_com_FisNano_FiscalMemory_GetFiscalRevolingAmount(JNIEnv *env, jobject thiz)
{
    uint8_t rd_buf[FlashManager::ENTRY_SIZE + 4];
    uint8_t get_len = FlashManager::ENTRY_SIZE;

    if(!CheckInit(env)) { return nullptr; }
    //
    jint ret = m_FiscalMem->GetFiscalRevolvingAmount(rd_buf, get_len);
    if(ret == FlashManager::CMD_OK && get_len == FlashManager::ENTRY_SIZE)
    {
        jbyteArray mByteArray = env->NewByteArray(FlashManager::ENTRY_SIZE);
        env->SetByteArrayRegion(mByteArray, 0, FlashManager::ENTRY_SIZE,
                                reinterpret_cast<const jbyte *>(rd_buf));
        return mByteArray;
    }

    return nullptr;
}

/*
 * Class:     com_FisNano_FiscalMemory
 * Method:    SetMode
 * Signature: ()I
 */
extern "C" JNIEXPORT jint JNICALL Java_com_FisNano_FiscalMemory_SetMode(JNIEnv *env, jobject thiz, jboolean enable_user_mode)
{
    if(!CheckInit(env)) { return FlashManager::CMD_HARDWARE_FAULT; }
    jint mode = enable_user_mode ? 0 : 1;
    return m_FiscalMem->SetMode((uint8_t)mode);
}


/*
 * Class:     com_FisNano_FiscalMemory
 * Method:    GetFirmwareInfo
 * Signature: ()[B
 */
extern "C" JNIEXPORT jstring JNICALL Java_com_FisNano_FiscalMemory_GetFirmwareInfo
  (JNIEnv * env, jobject self)
{
    char buf[200];
    if(!CheckInit(env)) { return nullptr; }
    str_fmInfo info =  m_FiscalMem->GetFmInfo();
    sprintf(buf, "type(%s) size(%u) version(%u), single_read_size(0x%x) single_write_size(0x%x) erase_size(0x%x)\n",
         info.fm_type, info.fm_size, info.fm_version, info.fm_single_read_size, info.fm_single_write_size,
         info.fm_erase_size);
    LOGD("type(%s) size(%u) version(%u), single_read_size(0x%x) single_write_size(0x%x) erase_size(0x%x)\n",
            info.fm_type, info.fm_size, info.fm_version, info.fm_single_read_size, info.fm_single_write_size,
            info.fm_erase_size);
    return env->NewStringUTF(buf);
}

/*
 * Class:     com_FisNano_FiscalMemory
 * Method:    GetDailySalesTotalSum
 * Signature: ()J
 */
extern "C" JNIEXPORT jlong JNICALL Java_com_FisNano_FiscalMemory_GetDailySalesTotalSum
  (JNIEnv * env, jobject self)
{
    if(!CheckInit(env)) { return FlashManager::CMD_HARDWARE_FAULT; }
    return m_FiscalMem->GetDailySalesTotal();
}

/*
 * Class:     com_FisNano_FiscalMemory
 * Method:    GetDailySalesTaxSum
 * Signature: ()J
 */
extern "C" JNIEXPORT jlong JNICALL Java_com_FisNano_FiscalMemory_GetDailySalesTaxSum
  (JNIEnv * env, jobject self)
{
    if(!CheckInit(env)) { return FlashManager::CMD_HARDWARE_FAULT; }
    return m_FiscalMem->GetDailySalesTaxTotal();
}

/*
 * Class:     com_FisNano_FiscalMemory
 * Method:    SetDailySalesTotalSumRangeByIndex
 * Signature: (IJ)Z
 */
extern "C" JNIEXPORT jint JNICALL Java_com_FisNano_FiscalMemory_SetDailySalesTotalSumRangeByIndex
  (JNIEnv * env, jobject self, jint Index, jint Range)
{
    if(!CheckInit(env)) { return FlashManager::CMD_HARDWARE_FAULT; }
    return m_FiscalMem->SetDailySalesTotalStartStop_Index((uint32_t)Index, (uint32_t)Range);
}



/*
 * Class:     com_FisNano_FiscalMemory
 * Method:    SetDailySalesTotalSumRangeByDateTime
 * Signature: (Ljava/util/Date;J)Z
 */
extern "C" JNIEXPORT jint JNICALL Java_com_FisNano_FiscalMemory_SetDailySalesTotalSumRangeByDateTime
  (JNIEnv * env, jobject self, jlong start_date, jlong end_date)
{
    if(!CheckInit(env)) { return FlashManager::CMD_HARDWARE_FAULT; }
    return m_FiscalMem->SetDailySalesTotalStartStop_DateTime(start_date, end_date);
}

/*
 * Class:     com_FisNano_FiscalMemory
 * Method:    DailySalesTotalSumRange
 * Signature: ()J
 */
extern "C" JNIEXPORT jlong JNICALL Java_com_FisNano_FiscalMemory_GetDailySalesTotalSumRange
  (JNIEnv * env, jobject self)
{
    if(!CheckInit(env)) { return FlashManager::CMD_HARDWARE_FAULT; }
    return m_FiscalMem->GetDailySalesTotal_Range();
}

/*
 * Class:     com_FisNano_FiscalMemory
 * Method:    DailySalesTaxSumRange
 * Signature: ()J
 */
extern "C" JNIEXPORT jlong JNICALL Java_com_FisNano_FiscalMemory_GetDailySalesTaxSumRange
  (JNIEnv * env, jobject self)
{
    if(!CheckInit(env)) { return FlashManager::CMD_HARDWARE_FAULT; }
    return m_FiscalMem->GetDailySalesTaxTotal_Range();
}



extern "C" JNIEXPORT jboolean JNICALL
Java_com_FisNano_FiscalMemory_GetFullStatus(JNIEnv *env, jobject thiz) {
    // TODO: implement GetFullStatus()
    if(!CheckInit(env)) { return JNI_FALSE; }
    return (jboolean)m_FiscalMem->GetFullStatus();
}

