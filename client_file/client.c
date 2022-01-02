#include "sock_init.h"
#include "utils.h"

int main(){
    struct sock_info server_info;
    if(read_conf(&server_info) != 1){
        return 0;
    }
    // 给ip port初始化socket
    int sock_fd = sock_init(server_info);
    if(-1 == sock_fd){
        printf("连接服务器%s失败！\n", server_info.ip);
        return 0;
    }
    while(1){
        char input_buff[128] = {0};
        printf("connect %s >> ", server_info.ip);
        fflush(stdout);

        fgets(input_buff, 128, stdin);
        // printf("strlen(buff) = %ld\n", strlen(input_buff));

        // 处理输入的\n，由于需要把字符串发给服务器直接执行，进行进程替换的时候不能带上回车
        input_buff[strlen(input_buff) - 1] = '\0';
        char send_buff[128] = {0};
        // 由于解析字符串用strtok会修改传入的字符串，用\0替换分隔符，所以需要保存一份字符串用于发送
        strcpy(send_buff, input_buff);

        char* args[ARG_MAX] = {0};
        char* cmd = get_cmd(input_buff, args);
        
        if(NULL == cmd){
            continue;
        }else if(strcmp(cmd, "exit") == 0){
            break;
        }else if(strcmp("download", cmd) == 0){
            send(sock_fd, send_buff, strlen(send_buff), 0);
            // 下载
            memset(log, 0, strlen(log));
            sprintf(log, "准备下载文件：%s\n", args[1]);
            write_log(log);

            if(recv_file(sock_fd, args[1]) == -1){
                memset(log, 0, strlen(log));
                sprintf(log, "下载失败！\n");
                write_log(log);
                printf("下载失败！\n");
                continue;
            }
        }else if(strcmp("upload", cmd) == 0){
            // 上传

        }else{
            // 和服务器交互
            send(sock_fd, send_buff, strlen(send_buff), 0);
            // 假设返回的数据不超过1024，后面可以优化，若server回复的数据较多，可以循环接收
            char recv_buff[1024] = {0};
            int res = recv(sock_fd, recv_buff, 1024, 0);
            // 服务器可能返回
            // 1. ok#
            // 2. ok#a.c b.c
            if(res <= 0){
                // 服务器断开
                memset(log, 0, strlen(log));
                sprintf(log, "服务器断开！\n");
                write_log(log);
                printf("服务器断开\n");
                break;
            }
            if(strncmp(recv_buff, SUCCEED_FLAG, strlen(SUCCEED_FLAG)) == 0){
                // 成功返回
                printf("%s\n", recv_buff + strlen(SUCCEED_FLAG));
            }
        }
    }

    close(sock_fd);

    return 0;
}
