#ifndef UTILS
#define UTILS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <time.h>

#define LOG_SEP " =====> "
#define LOG_FILE "log.txt"

#define FILE_MAX_SIZE (1024*1024)

void write_log(char* buffer);
char log[512];

#endif
