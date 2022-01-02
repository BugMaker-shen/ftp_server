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

#define ARG_MAX 10
#define SUCCEED_FLAG "ok#"
#define FILE_PATH "/home/shen/code/ftp_server/client_file/files/"
#define LOG_SEP " =====> "
#define LOG_FILE "log.txt"

void set_nonblock(int fd);
char* get_cmd(char* buff, char* args[]);
int recv_file(int conn, char* file_name);
void write_log(char* buffer);
char log[512];

#endif