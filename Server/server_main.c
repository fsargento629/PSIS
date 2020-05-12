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
    srand(time(0)); 
    player_connections = 0; 
    int i,nbytes;
    pthread_t accept_thread_id,fruit_thread_id,score_thread_id;
    int maxplayers=MAXPLAYERS;
    printf("Program started\n");


    printf("Assigning signal handlers\n");
    signal(SIGPIPE, signal_callback_handler);
    signal(SIGINT, signal_kill_handler);


    board_data=read_board_data(BOARDTXT);
    printf("Board size: %d %d\n",board_data.board_size[0],board_data.board_size[1]);
    int server_socket = init_server(DEFAULT_SERVER_PORT);

    for(i=0;i<MAXPLAYERS;i++)
        client_fd_list[i]=0;

    pthread_mutex_init(&board_lock, NULL);//mutex to control board use
    pthread_create(&accept_thread_id,NULL,accept_thread,&server_socket);

    //create fruit thread
    fruit_thread_args fruit_thread_arg;
    fruit_thread_arg.board=board;
    fruit_thread_arg.size_x=board_data.board_size[0];
    fruit_thread_arg.size_y=board_data.board_size[1];
    pthread_create(&fruit_thread_id,NULL,fruit_thread,&fruit_thread_arg);


    //initialize score vector and call score thread
    for(i=0;i<=maxplayers;i++)
        game_state.scores[i]=-1;


    pthread_create(&score_thread_id,NULL,accept_score_thread,&maxplayers);
    while(1){
        //update clients
        for(i=0;i<=MAXPLAYERS;i++){
            if(client_fd_list[i]!=0){
                
                nbytes=send_game_state(client_fd_list[i]);
                //printf("[Client updated] Sent %d bytes to client %d\n",nbytes,i);  
                if(nbytes <= 0)
                {
                    player_connections--;
                    //clear player
                    close(client_fd_list[i]);
                    client_fd_list[i] = 0;  
                }          
            }
        }
        usleep(10*1000);//x miliseconds
    }

    close(server_socket);
    printf("Server shutting down\n");
    return 0;
}
