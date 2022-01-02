/**
 * @file sock_init.h
 * @author BugMaker-shen
 * @brief 
 * @version 0.1
 * @date 2021-12-30
 * 
 * @copyright Copyright (c) 2021
 * 
 * 声明方法
 */

#ifndef __SOCK_INIT
#define __SOCK_INIT

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <assert.h>

int socket_init();

#endif
