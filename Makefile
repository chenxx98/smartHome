#程序版本号
VERSION = 1.0.0		
 
#定义变量，使用变量:$(变量名)
CC = gcc
 
#定义变量srcs，表示需要编译的源文件，需要表明路径，如果直接写表示这些cpp文件和makefile在同一个目录下，如果有多个源文件，每行以\结尾
#其中wildcard和patsubst为关键字不同目录下c文件，$(wildcard *.c /xxx/xxx/*.c), 替换所有的c为o表示 $(patsubst %c,%o,$(SRCS)) 
SRCS = $(wildcard ./*.c)
		
#将.c源文件编译成.o文件: 定义变量OBJS,表示将原文件中所有以.cpp结尾的文件替换成以.o结尾，
OBJS = $(patsubst %.c,%.o,$(SRCS))
 
#第三方库 头文件路径,实际编译把依赖头文件路径放入环境变量
INCLUDES = -I ../curl-7.71.1/_install/include/
 
#第三方库 文件路径,实际编译把依赖头文件路径放入环境变量
LIB_PATH = -L ../curl-7.71.1/_install/lib

#链接第三方库文件名字 	
LIBS = -lcurl 
 
#链接本地库
LOCLIBS = -lwiringPi -lpthread -lssl -lcrypto

#宏定义 在代码调试的过程中，我们通常会加个宏定义来控制此段代码是否被编译
DEBUG = -D_MACRO
 
#编译标志位
CFLAGS = -Wall -c	
 
#定义变量，表示最终生成的可执行文件名
TARGET = run
 
#生成最终可执行文件
$(TARGET): $(OBJS)	
	$(CC) -std=c99 $(OBJS)  $(LIB_PATH) $(LIBS) $(LOCLIBS) -o $(TARGET)
 
#所有的[.c]文件都编译成[.o]文件
%.o: %.c
	$(CC)   $(INCLUDES) $(LIB_PATH) $(LIBS) $(DEBUG) $(CFLAGS) $(LOCLIBS) $< -o $@  -w
 
#伪目标 执行# make clean指令删除*.o文件
.PHONY: clean
clean:
	rm -rf $(OBJS) 
	
 
