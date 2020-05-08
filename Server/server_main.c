#include <sys/types.h>  
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include"server.h"
#include <sys/un.h>
#include <signal.h>
#include <unistd.h>


 
int main(int argc,char*argv[]){
    player_connections = 0; 
    int i,nbytes;
    pthread_t accept_thread_id;
    printf("Program started\n");
    printf("Assigning signal handlers\n");
    signal(SIGPIPE, signal_callback_handler);
    signal(SIGINT, signal_kill_handler);
    board_data=read_board_data(BOARDTXT);
    printf("Board size: %d %d\n",board_data.board_size[0],board_data.board_size[1]);
    int server_socket = init_server();

    for(i=0;i<MAXPLAYERS;i++)
        client_fd_list[i]=0;

    pthread_create(&accept_thread_id,NULL,accept_thread,&server_socket);

    while(1){
        
        //update clients
        for(i=0;i<=MAXPLAYERS;i++){
            if(client_fd_list[i]!=0){
                
                nbytes=send_game_state(client_fd_list[i]);
                //printf("[Client updated] Sent %d bytes to client %d\n",nbytes,i);  
                if(nbytes <= 0)
                {
                    player_connections--;
                    close(client_fd_list[i]);
                    client_fd_list[i] = 0;  
                }          
            }
        }
        usleep(50*1000); 
    }

    close(server_socket);
    printf("Server shutting down\n");
    return 0;
}
