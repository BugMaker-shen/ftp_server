#include "sock_init.h"
#include "work_thread.h"

int main(){
    int sock_fd = socket_init();
    if(sock_fd == -1){
        exit(0);
    }
    
    while(1){
        struct sockaddr_in cli_addr;
        int len = sizeof(cli_addr);
        
        int conn = accept(sock_fd, (struct sockaddr*)&cli_addr, &len);
        if(conn < 0){
            continue;
        }
        printf("客户%d连接\n", conn);
        
        // 启动服务线程
        thread_start(conn);
    }
    return 0;
}
