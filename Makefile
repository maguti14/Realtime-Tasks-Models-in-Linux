# the compiler: gcc for C program, define as g++ for C++
#CC = gcc


CFLAGS  = -std=gnu11 -Wall
# the build target executable:
TARGET = homework1

native:
	gcc $(CFLAGS) -o $(TARGET) $(TARGET).c $(TARGET).h -pthread


SDK_HOME = /opt/iot-devkit/1.7.2/sysroots

PATH := $(PATH):$(SDK_HOME)/x86_64-pokysdk-linux/usr/bin/i586-poky-linux

CC = i586-poky-linux-gcc
ARCH = x86
CROSS_COMPILE = i586-poky-linux-
SROOT=$(SDK_HOME)/i586-poky-linux/

cross:
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c $(TARGET).h -pthread --sysroot=$(SROOT)


clean:
	$(RM) $(TARGET)
