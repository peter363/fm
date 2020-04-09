LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := FiscalMemory
LOCAL_SRC_FILES := com_FisNano_FiscalMemory.cpp Serial.cpp SerialMonitor.cpp ProtocolParser.cpp OtpMonitor.cpp Crc.cpp Configuration.cpp FlashManager.cpp
LOCAL_LDLIBS:=-L$(SYSROOT)/usr/lib -llog
include $(BUILD_SHARED_LIBRARY)

