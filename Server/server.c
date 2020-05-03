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

board_data_struct read_board_data(char* file_name){
    int size_x,size_y;
    char buff[100];
    FILE* fp=fopen(file_name,"r");
    board_data_struct ret;
    if(fp==NULL){
        printf("%s not found\n",file_name);
        exit(-1);
    }

    fgets(buff,sizeof(buff),fp);
    sscanf(buff,"%d %d",&size_x,&size_y);

    ret.board_size[0]=size_x;
    ret.board_size[1]=size_y;

    

    return ret;

}

int send_initial_message(int client_fd,int player_num){
    setup_message msg;
    msg.game_state=game_state;
    msg.player_num=player_num;
    msg.board_size[0]=board_data.board_size[0];
    msg.board_size[1]=board_data.board_size[1];

    //printf("Sent %d %d\n",msg.board_size[0],msg.board_size[1]);
    int nbytes= send(client_fd,&msg,sizeof(setup_message),0); //critical!!
    //printf("%d %d %d\n",msg.board_size[0],msg.board_size[1],msg.player_num);
    return nbytes;
}

void update_board(int player,C2S_message msg){  
    //move player 
    int i=0;
    while(game_state.objects[i].player!=player && game_state.objects[i].type!=msg.type){
        i++;
    }
    //update the cell
    game_state.objects[i].pos[0]=msg.x;
    game_state.objects[i].pos[1]=msg.y;

}




void init_server(){
    struct sockaddr_in server_local_addr;
    server_socket=socket(AF_INET,SOCK_STREAM,0);
    if(server_socket==-1){
        perror("socket:");
        exit(-1);
    }
    server_local_addr.sin_family=AF_INET;
    server_local_addr.sin_addr.s_addr=INADDR_ANY;
    server_local_addr.sin_port=htons(3000);
    int err=bind(server_socket,(struct sockaddr*)&server_local_addr,
                sizeof(server_local_addr));
    if(err==-1){
        perror("bind:");
        exit(-1);
    }

    // should we do the listen here? idont think so

}

void init_player_position(int player_num){
    int x,y,i;
    //create a pacman and a monster
    for(i=0;i<MAXOBJECTS;i++){
        if(game_state.objects[i].type=0){
            game_state.objects[i].type=1;
            game_state.objects[i].color=DEFAULT_COLOR;
            game_state.objects[i].player=player_num;
            game_state.objects[i].pos[0]=rand()%board_data.board_size[0];
            game_state.objects[i].pos[1]=rand()%board_data.board_size[1];
            
        }
    }

   while(game_state.objects[i].type!=0){
       i++;
   }

    //create misnter
    game_state.objects[i].type=2;
    game_state.objects[i].color=DEFAULT_COLOR;
    game_state.objects[i].player=player_num;
    game_state.objects[i].pos[0]=rand()%board_data.board_size[0];
    game_state.objects[i].pos[1]=rand()%board_data.board_size[1];
}

// Thread that receives updates from each client and triggers an event (?)
void* client_thread(void* client_args){
    client_thread_args args = *(client_thread_args*)client_args;
    int client_fd=args.fd;
    int player_num=args.player_num;
    init_player_position(player_num);
    send_initial_message(client_fd,player_num);
    client_fd_list[player_num]=client_fd;
    int err_rcv;
    C2S_message msg;
    while((err_rcv = recv(client_fd,&msg,sizeof(msg),0))>0 ){
        printf("received %d bytes\n",err_rcv);
        // handle message from client
        update_board(player_num,msg);

    }


}

void* accept_thread(void*arg){
    client_thread_args client_data;
    struct sockaddr_in client_addr;
    socklen_t size_addr = sizeof(client_addr);
    pthread_t client_thread_ids[MAXPLAYERS];
    int client_fd,i=0;
    if(listen(server_socket,MAXPLAYERS)==-1){
        perror("listen");
        exit(-1);
    }
    printf("Waiting for connections\n");
    i=0;
    while(1){
        client_fd=accept(server_socket,(struct sockaddr*)&client_addr,&size_addr);
        if(client_fd==-1){
            perror("accept:");
            exit(-1);
        }
        printf("Accepted new connection\n");
        
        client_data.fd=client_fd;
        client_data.player_num=i;      
        pthread_create(&client_thread_ids[i],NULL,client_thread,&client_data);
        i++;
    }

}

int update_clients(){
    int i;
    S2C_message msg;
    msg.game_state=game_state;
    for(i=0;i<MAXPLAYERS;i++){
        if(client_fd_list[i]==0)
            continue;
        else
        {
           //send to server
          
           write(client_fd_list[i],&msg,sizeof(S2C_message));
        }
        
    }
    return 0;
}



