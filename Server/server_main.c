#include <sys/types.h>  
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include"server.h"
#include <sys/un.h>
#include <unistd.h>


int main(int argc,char*argv[]){
    pthread_t accept_thread_id;
    board_data=read_board_data(BOARDTXT);
    init_server();
    pthread_create(&accept_thread_id,NULL,accept_thread,NULL);

    while(1){
        update_clients();
    }

    printf("Server shutting down\n");
    return 0;
}
