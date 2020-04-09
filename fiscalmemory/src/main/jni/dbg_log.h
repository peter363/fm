//
// Created by Administrator on 2020/2/23 0023.
//

#ifndef SERIALDEMO_DBG_LOG_H
#define SERIALDEMO_DBG_LOG_H

#ifdef DEBUG_PC_MONITOR

#include <iostream>
#include <stdio.h>
// 定义info信息
#define LOGI(...) printf("[I] " TAG ": " __VA_ARGS__)

// 定义debug信息
#define LOGD(...) printf("[D] " TAG ": " __VA_ARGS__)

// 定义error信息
#define LOGE(...) printf("[E] " TAG ": " __VA_ARGS__)

#define LOG_RAW(...) printf(__VA_ARGS__)
#else
// 引入log头文件
#include  <android/log.h>
// 定义info信息
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,TAG,__VA_ARGS__)

// 定义debug信息
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)

// 定义error信息
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)

#define LOG_RAW(...) __android_log_print(ANDROID_LOG_INFO,TAG,__VA_ARGS__)

#endif

#endif //SERIALDEMO_DBG_LOG_H
