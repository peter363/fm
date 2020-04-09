//
// Created by Administrator on 2020/2/27 0027.
//

#ifndef SERIALDEMO_FMCONTEXT_H
#define SERIALDEMO_FMCONTEXT_H

#ifdef DEBUG_PC_MONITOR
typedef void JNIEnv; 
#else
#include <jni.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif
typedef struct FM_CONTEXT
{
    JNIEnv * env;
}FM_CONTEXT_t;


#ifdef __cplusplus
}
#endif

#endif //SERIALDEMO_FMCONTEXT_H
