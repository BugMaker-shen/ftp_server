// 防止头文件里的变量重复定义，若这里定义了结构体或者其他变量，其他文件include一次，就被重复定义了
// 加上预编译指令后，第二次include本文件的时候，就不会重复定义了
#ifndef __THREAD_START
#define __THREAD_START

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <pthread.h>

void thread_start(int conn);

#endif
