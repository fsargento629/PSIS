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

int init_player_position(int player_num){
    //give the player a pacman and a monster in random, not filled position:
    int x=0,y=0;
    do
    {
        x=rand()%board_size[0];
        y=rand()%board_size[1];
    } while(!is_empty(x,y,board));
    //create pacman
    board[y][x].player=player_num;
    board[y][x].type=1;
    board[y][x].pos[0]=x;
    board[y][x].pos[1]=y;
    board[y][x].color=1;
        
    do
    {
        x=rand()%board_size[0];
        y=rand()%board_size[1];
    } while(!is_empty(x,y,board));
    //create monster
    board[y][x].player=player_num;
    board[y][x].type=2;
    board[y][x].pos[0]=x;
    board[y][x].pos[1]=y;
    board[y][x].color=1;

    return 0;//if found  space for both the pacman and moster

}




board_data_struct read_board_data(char*file_name){
    board_data_struct board_data;
    //game_object_struct** board;
    int i_x,i_y;
    int x=0,y=0;
    char buff[500],ch;
    FILE* fp=fopen("board.txt","r");
    fgets(buff,sizeof(buff),fp);
    sscanf(buff,"%d %d",&x,&y);
    board_data.board_size[0]=x;
    board_data.board_size[1]=y;
    board=malloc(sizeof(game_object_struct*)*y);
    // read bricks:
    i_x=0;
    i_y=0;
    board[i_y]=malloc(sizeof(game_object_struct)*x);
    while ((ch = fgetc(fp)) != EOF)
    {
        if(ch=='\n'){
            //printf("  (%d,%d)\n",i_x,i_y);
            i_x=0;;
            i_y++;;
            board[i_y]=malloc(sizeof(game_object_struct)*x);
                        
        }
        else if(ch=='B'){
            board[i_y][i_x].color=0;
            board[i_y][i_x].player=-1;
            board[i_y][i_x].pos[0]=i_x;
            board[i_y][i_x].pos[1]=i_y;
            board[i_y][i_x].type=3;//brick
            //printf("B");
            i_x++;
        }
        else{
            board[i_y][i_x].color=0;
            board[i_y][i_x].player=-1;
            board[i_y][i_x].pos[0]=i_x;
            board[i_y][i_x].pos[1]=i_y;
            board[i_y][i_x].type=0;//empty
            //printf("_");
            i_x++;
        }

        board_data.board=board;
        
           

    }
    
    return board_data;

}
//function that sends a certain client all the game_state data (board and scores)
// returns the number of bytes sent
int send_game_state(int client_fd){
    int nbytes,Nbytes=0;
    int i;
    
    //Send board, line by line:
    for(i=0;i<board_size[1];i++){
            nbytes=send(client_fd,board[i],sizeof(game_object_struct)*board_size[0],0);
            Nbytes=nbytes++; 
    }
    //Now send the score vector
    nbytes=send(client_fd,game_state.scores,sizeof(int)*MAXPLAYERS,0);
    Nbytes=nbytes++;

    return Nbytes;

}



int send_initial_message(int client_fd,int player_num){

    /* 
    Initial message protocol:
    1) Send player_num and board size to client
    2) Send board to client, line by line by calling the function send_game_state
    3) send  the scores
    
    */


    int nbytes,Nbytes=0;
    setup_message msg;
    msg.player_num=player_num;
    msg.board_size[0]=board_data.board_size[0];
    msg.board_size[1]=board_data.board_size[1];
    // send 1st part of the initial message
    nbytes=send(client_fd,&msg,sizeof(setup_message),0);
    Nbytes=nbytes++;
    //Send seconde part of the message:
    nbytes=send_game_state(client_fd);
    Nbytes=nbytes++;
    printf("[ClientSetup] Server sent %d bytes to client %d\n ",Nbytes,player_num);
    //Message has been sent. 
    return Nbytes;
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
//this function updates the board after a client request
void update_board(int player_num,C2S_message msg){
    int* pos;
    //find matching pacman or moster and update its position
    pos=find_object(player_num,msg.type,board,board_size[0],board_size[1]);
    //move the player:
    board[msg.y][msg.x].color=board[pos[1]][pos[0]].color;
    board[msg.y][msg.x].player=player_num;
    board[msg.y][msg.x].type=msg.type;
    board[msg.y][msg.x].pos[0]=msg.x;
    board[msg.y][msg.x].pos[1]=msg.y;
    //clear previous position
    clear_board_cell(pos[0],pos[1],board);
    
}


// Thread that receives updates from each client and triggers an event (?)
void* client_thread(void* client_args){
    client_thread_args args = *(client_thread_args*)client_args;
    int client_fd=args.fd;
    int player_num=args.player_num;
    init_player_position(player_num);//initiate player position
    send_initial_message(client_fd,player_num); 
    client_fd_list[player_num]=client_fd;
    int err_rcv;
    C2S_message msg;
    while((err_rcv = recv(client_fd,&msg,sizeof(msg),0))>0 ){
        printf("[Client request] Received %d bytes from client %d \n",err_rcv,player_num);
        // handle message from client
        update_board(player_num,msg);

    }


}

void* accept_thread(void*arg){ //args is empty
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
        printf("Accepted new client (id=%d) from %d\n",i,client_fd);
        
        client_data.fd=client_fd;
        client_data.player_num=i;      
        pthread_create(&client_thread_ids[i],NULL,client_thread,&client_data);
        i++;
    }

}





