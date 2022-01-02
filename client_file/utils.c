#include "utils.h"

void set_nonblock(int fd){
    // 获取文件描述符fd的属性
    int old_feature = fcntl(fd, F_GETFL);
    int new_feature = old_feature | O_NONBLOCK;
    // 将新属性设置到fd
    if(fcntl(fd, F_SETFL, new_feature) == -1){
        perror("fcntl error\n");
    }
}

char* get_cmd(char* buff, char* args[]){
    if(NULL == buff || NULL == args){
        return NULL;
    }
    
    int i = 0;
    char* str = strtok(buff, " ");
    while(NULL != str){
        args[i++] = str;
        str = strtok(NULL, " ");
    }
    // 最后args里面的指针，指向的就是input_buff的地址

    // 查看 msg arg[0]的地址
    // printf("%p\n", buff);
    // for(int i = 0; i < 10; i++){
    //     printf("%p\n", args[i]);
    // }
    return args[0];
}

// 解析字符串，分析是否能够下载
// 可下载返回文件大小，不可下载返回-1
int parse_str(char* buff, char* file_name, mode_t* mode){
    // 解析的结果存入args
    char* args[ARG_MAX] = {0};

    int i = 0;
    char* str = strtok(buff, "#");
    while(NULL != str){
        args[i++] = str;
        str = strtok(NULL, "#");
    }

    if(strcmp(args[0], "ok") == 0){
        *mode = atoi(args[2]);
        printf("%s大小为：%sB, %s:", file_name, args[1], args[3]);
        fflush(stdout);
        return atoi(args[1]);
    }else{
        printf("%s\n", args[0]);
        return -1;
    }

}

// 从conn接收文件，存入FILE_PATH
// 成功返回1，失败返回-1
int recv_file(int conn, char* file_name){
    memset(log, 0, strlen(log));
    sprintf(log, "准备接收下载信息\n");
    write_log(log);

    char buff[128] = {0};
    // buff存放server返回的下载信息
    recv(conn, buff, 127, 0);
    mode_t mode = 0;
    int file_size = parse_str(buff, file_name, &mode);
    
    memset(buff, 0, 128);
    fgets(buff, 128, stdin);
    buff[strlen(buff) - 1] = '\0';
    send(conn, buff, strlen(buff), 0);

    if(file_size == -1){
        // 不可下载，退出
        return -1;
    }
    // 可以下载，需要输入是否下载，并且根据server回复的字节数收文件
    char data[1024] = {0};
    char path[128] = {0};
    // 拼接下载路径
    strcpy(path, FILE_PATH);
    strcat(path, file_name);
    
    int fp = open(path, O_CREAT | O_WRONLY, mode);
    if(fp == -1){
        // printf("文件创建异常！\n");
        memset(log, 0, strlen(log));
        sprintf(log, "文件创建异常！\n");
        write_log(log);
        return -1;
    }
    // printf("文件创建成功！\n");
    
    int down_size = 0;
    while(down_size < file_size){
        int n = recv(conn, data, 1023, 0);
        down_size += n;
        // 动态打印进度条
        printf("当前下载进度：%.1f%%\r", down_size * 1.0 / file_size * 100);
        fflush(stdout);
        if(down_size >= file_size){
          printf("当前下载进度：%.1f%%\n", down_size * 1.0 / file_size * 100);
        }
        write(fp, data, n);
    }
    memset(log, 0, strlen(log));
    sprintf(log, "文件%s接收完成！\n", file_name);
    write_log(log);
    close(fp);

    return 1;
}


#define FILE_MAX_SIZE (1024*1024)
 
/*
获得当前时间字符串
@param buffer [out]: 时间字符串
@return 空
*/
void get_local_time(char* buffer){
	time_t rawtime; 
	struct tm* timeinfo;
	time(&rawtime); 
	timeinfo = localtime(&rawtime); 
	sprintf(buffer, "%04d-%02d-%02d %02d:%02d:%02d", 
		(timeinfo->tm_year+1900), timeinfo->tm_mon, timeinfo->tm_mday,
		timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
}

/*
获得文件大小
@param filename [in]: 文件名
@return 文件大小
*/
long get_file_size(char* filename){
	long length = 0;
	FILE *fp = NULL;
 
	fp = fopen(filename, "r");
	if (fp != NULL){
		fseek(fp, 0, SEEK_END);
		length = ftell(fp);
        fseek(fp, 0, SEEK_SET);
	}

	if (fp != NULL){
		fclose(fp);
		fp = NULL;
	}
 
	return length;
}
 
/*
写入日志文件
@param filename [in]: 日志文件名
@param max_size [in]: 日志文件大小限制
@param buffer [in]: 日志内容
@param buf_size [in]: 日志内容大小
@return 空
*/
void write_log_file(char* filename, long max_size, char* buffer, unsigned buf_size){
	if (filename != NULL && buffer != NULL){
		// 文件超过最大限制, 删除
		long length = get_file_size(filename);
 
		if (length > max_size){
			unlink(filename); // 删除文件
		}
 
		// 写日志
		{
			FILE *fp;
			fp = fopen(filename, "a+");
			if (fp != NULL){
				char now[32];
				memset(now, 0, sizeof(now));
				get_local_time(now);
				fwrite(now, strlen(now)+1, 1, fp);
                fwrite(LOG_SEP, strlen(LOG_SEP), 1, fp);
				fwrite(buffer, buf_size, 1, fp);

				fclose(fp);
				fp = NULL;
			}
		}
	}
}

void write_log(char* buffer){
    write_log_file(LOG_FILE, FILE_MAX_SIZE, buffer, strlen(buffer));
}