#include "work_thread.h"
#include "utils.h"

#define ARG_MAX 10
#define SUCCEED_FLAG "ok#"

// 将msg的分割结果用arg的每个元素（指针）指向
// 并且返回cmd
char* get_cmd(char* msg, char* args[]){
    if(NULL == msg || NULL == args){
        return NULL;
    }

    char* p = NULL;
    // 线程安全分割，p记录当前分割的位置
    int i = 0;
    char* str = strtok_r(msg, " ", &p);
    while(NULL != str){
        args[i++] = str;
        str = strtok_r(NULL, " ", &p);
    }

    // 查看 msg arg[0]的地址
    // printf("msg = %p\n", msg);
    // printf("arg[0] = %p\n", args[0]);
    return args[0];
}

// 成功返回文件描述符并将文件大小带出，失败返回-1
int open_file(char* file_name, int* file_size, mode_t* mode){
    if(NULL == file_name){
        return -1;
    }
    int fp = open(file_name, O_RDONLY);
    if(-1 == fp){
        return -1;
    }
    
    struct stat file_info = {0};
    stat(file_name, &file_info);
    *mode = file_info.st_mode;

    // off_t lseek(int fd, off_t offset, int whence);
    // lseek返回文件fd中，whence移动offset后相对于文件开头的偏移量
    *file_size = lseek(fp, 0, SEEK_END);
    lseek(fp, 0, SEEK_SET);
    return fp;
}

void send_file(int conn, char* file_name){
    int file_size = 0;
    mode_t mode = 0;

    int fp = open_file(file_name, &file_size, &mode);
    if(fp == -1){
        // 如果不能下载，需要给客户回复，否则客户端阻塞在recv
        memset(log, 0, strlen(log));
        sprintf(log, "文件打开失败\n");
        write_log(log);

        send(conn, "文件打开失败", strlen("文件打开失败"), 0);
        return;
    }

    char buff[128] = {0};
    // 格式化字符串输出到指定位置
    sprintf(buff, "ok#%d#%d#回复yes进行下载，回复no取消下载", file_size, mode);
    // 告诉客户端文件的大小以及下载信息
    send(conn, buff, strlen(buff), 0);
    while(1){
        memset(buff, 0, 128);
        int n = recv(conn, buff, 127, 0);
        memset(log, 0, strlen(log));
        sprintf(log, "用户的确认信息：%s\n", buff);
        write_log(log);

        // printf("用户的确认信息：%s\n", buff);

        // 把\n换成\0后，可以用strcmp进行比较
        // buff[strlen(buff) - 1] = '\0';

        if(n <= 0){
            break;
        }
        // 验证客户是否下载
        if(strcmp(buff, "y") == 0 || strcmp(buff, "yes") == 0){
            int num = 0;
            char data[1024] = {0};
            while((num = read(fp, data, 1024)) > 0){
                send(conn, data, num, 0);
            }
            memset(log, 0, strlen(log));
            sprintf(log, "发送文件%s完成！\n", file_name);
            write_log(log);

            break;
        }else if(strcmp(buff, "n") == 0 || strcmp(buff, "no") == 0){
            break;
        }else{
            // 用户输入不合法
            char* prompt_msg = "输入不合法，回复yes进行下载，回复no取消下载";
            send(conn, prompt_msg, strlen(prompt_msg), 0);
        }
    }
    close(fp);
}

// 服务函数
void* work_thread(void* arg){
    
    int conn = (int)arg;
    memset(log, 0, strlen(log));
    sprintf(log, "给%d服务的线程启动\n", conn);
    write_log(log);

    if(conn < 0){
        // 检查文件描述符合法
        pthread_exit(NULL);
    }
    while(1){
        char msg[128] = {0};
        int num = recv(conn, msg, 127, 0);
        if(num > 0){
            memset(log, 0, strlen(log));
            sprintf(log, "收到客户端：%s\n", msg);
            write_log(log);
        }

        if(num <= 0){
            break;
        }
        char* args[ARG_MAX] = {0};
        
        // 将msg的分割结果用arg的每个元素（指针）指向
        char* cmd = get_cmd(msg, args);

        if(NULL == cmd){
            //由于按照空格解析，若解析的命令为空，则说明客户输入的不正确
            memset(log, 0, strlen(log));
            sprintf(log, "命令有误\n");
            write_log(log);

            send(conn, "命令有误\n", strlen("命令有误\n"), 0);
            continue;
        }
        if(strcmp(cmd, "download") == 0){
            // 下载文件，目前只考虑下载单个文件
            send_file(conn, args[1]);
        }else if(strcmp(cmd, "upload") == 0){
            // 上传文件

        }else{
            // 交互
            // fork + exec
            // read : pipe_fd[0], write : pipe_fd[1]
            // printf("开始交互\n");

            int pipe_fd[2];
            if(pipe(pipe_fd) == -1){
                send(conn, "管道创建失败\n", strlen("管道创建失败\n"), 0);
                continue;
            }
            pid_t pid = fork();
            if(pid == -1){
                send(conn, "fork子进程失败\n", strlen("fork子进程失败\n"), 0);
                continue;
            }
            if(pid == 0){
                // 子进程
                close(pipe_fd[0]);
                // 用管道的写端覆盖stdout，往stdout写数据，就相当于往无名管道写数据
                // 这里dup2的参数为int类型，是文件描述符，不能用stdout、stdin，这俩适用于FILE*类型的参数
                dup2(pipe_fd[1], 1);
                dup2(pipe_fd[1], 2);

                execvp(cmd, args);
                perror("进程替换失败，即命令有误\n");

                close(pipe_fd[1]);
                exit(0);
            }
            // 父进程
            
            close(pipe_fd[1]);
            wait(NULL);
            // printf("父进程部分，子进程结束\n");
            char read_pipe[1024] = {SUCCEED_FLAG};
            // 父进程从管道里读取子进程的执行结果，返回给用户
            read(pipe_fd[0], read_pipe + strlen(SUCCEED_FLAG), 1024 - strlen(SUCCEED_FLAG));
            // printf("管道内容：%s\n", read_pipe);
            send(conn, read_pipe, strlen(read_pipe), 0);
            close(pipe_fd[0]);
        }
    }
    memset(log, 0, strlen(log));
    sprintf(log, "client %d exit\n", conn);
    write_log(log);

    printf("client %d exit\n", conn);

    close(conn);
}

void thread_start(int conn){
    pthread_t id;
    pthread_create(&id, NULL, work_thread, (void*)conn);
}
