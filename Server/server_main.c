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
    printf("Board size: %d %d\n",board_data.board_size[0],board_data.board_size[1]);
    init_server();
    pthread_create(&accept_thread_id,NULL,accept_thread,NULL);

    while(1){
        usleep(100);
        update_clients();
    }

    printf("Server shutting down\n");
    return 0;
}
