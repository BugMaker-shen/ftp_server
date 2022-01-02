#include "sock_init.h"
#define IP    "ip"
#define PORT  "port"

char log[512] = {0};

// 失败返回0，成功返回1
int read_conf(struct sock_info* info){
    if(NULL == info){
        memset(log, 0, strlen(log));
        sprintf(log, "配置文件不存在\n");
        write_log(log);
        printf("配置文件不存在\n");
        return 0;
    }
    FILE* fp = fopen("conn_info.conf", "r");
    if(NULL == fp){
        memset(log, 0, strlen(log));
        sprintf(log, "读取conn_info.conf失败\n");
        write_log(log);
        printf("读取conn_info.conf失败\n");
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
        }else{
            memset(log, 0, strlen(log));
            sprintf(log,"第%d行不识别的参数：%s\n", line, s);
            write_log(log);
            printf("第%d行不识别的参数：%s\n", line, s);
        }
    }
    fclose(fp);
    return 1;
}

int sock_init(struct sock_info server_info){
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(sock_fd == -1){
        return -1;
    }

    struct sockaddr_in cli_addr;
    memset(&cli_addr, 0, sizeof(cli_addr));
    cli_addr.sin_family = AF_INET;
    cli_addr.sin_port = htons(server_info.port);
    cli_addr.sin_addr.s_addr = inet_addr(server_info.ip);

    int res = connect(sock_fd,(struct sockaddr*)&cli_addr,sizeof(cli_addr));
    if(res == -1){
        return -1;
    }
    return sock_fd;
}
