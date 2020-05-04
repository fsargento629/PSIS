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
    int i,nbytes;
    pthread_t accept_thread_id;
    board_data=read_board_data(BOARDTXT);
    printf("Board size: %d %d\n",board_data.board_size[0],board_data.board_size[1]);
    init_server();
    for(i=0;i<MAXPLAYERS;i++)
        client_fd_list[i]=0;
    pthread_create(&accept_thread_id,NULL,accept_thread,NULL);

    while(1){
        
        //update clients
        for(i=0;i<MAXPLAYERS;i++){
            if(client_fd_list[i]!=0){
                
                nbytes=send_game_state(client_fd_list[i]);
                printf("[Client updated] Sent %d bytes to client %d\n",nbytes,i);            
            }
        }
        usleep(100*1000); 
    }

    close(server_socket);
    printf("Server shutting down\n");
    return 0;
}
