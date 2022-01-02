/**
 * @file sock_init.c
 * @author BugMaker-shen
 * @brief 
 * @version 0.1
 * @date 2021-12-30
 * 
 * @copyright Copyright (c) 2021
 * 
 *  1.完成读取配置文件
 *  2.创建监听套接字
 */

#include "sock_init.h"
#define IP           "ip"
#define PORT         "port"
#define LISTEN_QUEUE "listen_queue"

// 由于sock_info只本文件中使用，所以没有定义在头文件中
struct sock_info{
    char ip[32];
    short port;
    int listen_queue;
};

// 失败返回0，成功返回1
int read_conf(struct sock_info* info){
    if(NULL == info){
        printf("配置文件不存在\n");
        return 0;
    }
    FILE* fp = fopen("sock_info.conf", "r");
    if(NULL == fp){
        printf("读取sock_info.conf失败\n");
        return 0;
    }

    int line = 0;
    while(1){
        char buff[128] = {0};
        // char *fgets(char *s, int size, FILE *stream);
        // fgets()  reads  in  at most one less than size characters from stream and stores them into the buffer pointed to by s.  Reading stops after an EOF or a newline. 
        // 遇到\n停止，但是包括\n
        char* s = fgets(buff, 128, fp);
        if(NULL == s){
            // 读取完成
            break;
        }
        line++;
        
        if('#' == s[0] || '\n' == s[0]){
            continue;
        }
        // 由于读取的结果包括\n，把最后的\n改为\0，否则info->ip包含\n
        if(s[strlen(s) - 1] == '\n'){
            s[strlen(s) - 1] = '\0';
        }

        if(strncmp(s, IP, strlen(IP)) == 0){
            strcpy(info->ip, s + strlen(IP) + 1);

        }else if(strncmp(s, PORT, strlen(PORT)) == 0){
            info->port = atoi(s + strlen(PORT) + 1);
            
        }else if(strncmp(s, LISTEN_QUEUE, strlen(LISTEN_QUEUE)) == 0){
            info->listen_queue = atoi(s + strlen(LISTEN_QUEUE) + 1);
            
        }else{
            printf("第%d行不识别的参数：%s", line, s);
        }
    }
    fclose(fp);
    return 1;
}

int socket_init(){
    struct sock_info info;
    if(read_conf(&info) != 1){
        return 0;
    }

    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(sock_fd == -1){
        printf("服务器创建socket失败\n");
        return -1;
    }

    struct sockaddr_in ser_addr;
    memset(&ser_addr, 0, sizeof(ser_addr));
    ser_addr.sin_family = AF_INET;
    ser_addr.sin_port = htons(info.port);
    ser_addr.sin_addr.s_addr = inet_addr(info.ip);

    int res = bind(sock_fd, (struct sockaddr*)&ser_addr, sizeof(ser_addr));
    if(res == -1){
        printf("服务器bind失败\n");
        return -1;
    }

    if(listen(sock_fd, info.listen_queue) == -1){
        printf("服务器listen失败\n");
        return -1;
    }

    printf("服务器%s:%d启动\n", info.ip, info.port);
    return sock_fd;
}
