#ifndef __SOCK_INIT
#define __SOCK_INIT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>

struct sock_info{
    char ip[32];
    short port;
};


int sock_init();
int read_conf(struct sock_info* info);

#endif
