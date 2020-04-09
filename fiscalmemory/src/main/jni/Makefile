#定义变量，使用变量:$(变量名)
CC=g++


SRCS=main.cpp\
    SerialMonitor.cpp ProtocolParser.cpp Serial.cpp OtpMonitor.cpp Crc.cpp Configuration.cpp

CPPFLAGS=-DDEBUG_PC_MONITOR
OBJS=$(SRCS:.cpp=.o) 
# INCLUDES_PATH=/mnt/d/Android/sdk/ndk/20.0.5594570/toolchains/llvm/prebuilt/windows-x86_64/sysroot/usr/include/
INCLUDES_PATH= 

EXEC=FmTest
start:$(OBJS)
		$(CC) $(CPPFLAGS) $(INCLUDES_PATH) -o $(EXEC) $(OBJS)
.cpp.o:
		$(CC) $(CPPFLAGS) $(INCLUDES_PATH) -o $@ -c $< -DMYLINUX

clean:
		rm -rf $(OBJS)


