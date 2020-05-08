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
#include <time.h>




int init_player_position(int player_num,int do_pacman,int do_monster){
    //give the player a pacman and a monster in random, not filled position:
    int x=0,y=0;
    int *pos;
    if(do_pacman==1){
        do
        {
            x=rand()%board_size[0];
            y=rand()%board_size[1];
        } while(!is_empty(x,y,board));
        //if there is a pacman of the same player anywhere, delet it
        pos=find_object(player_num,PACMAN,board,board_size[0],board_size[1]);
        if(pos[0]!=-1&&pos[1]!=-1)//if it is found, delete it
            clear_board_cell(pos[0],pos[1],board);
        free(pos);
        //create pacman
        board[y][x].player=player_num;
        board[y][x].type=1;
        board[y][x].pos[0]=x;
        board[y][x].pos[1]=y;
        board[y][x].color=1;
        
        
    }
    
    if(do_monster==1){
        do
        {
            x=rand()%board_size[0];
            y=rand()%board_size[1];
        } while(!is_empty(x,y,board));
        //if there is a monster of the same player anywhere, delete it
        pos=find_object(player_num,MONSTER,board,board_size[0],board_size[1]);
        if(pos[0]!=-1&&pos[1]!=-1)//if it is found, delete it
            clear_board_cell(pos[0],pos[1],board);
        free(pos);
        //create monster
        board[y][x].player=player_num;
        board[y][x].type=2;
        board[y][x].pos[0]=x;
        board[y][x].pos[1]=y;
        board[y][x].color=1;
    }
        
   
    return 0;//if found  space for both the pacman and moster

}

board_data_struct read_board_data(char*file_name){
    //board_data_struct board_data;
    //game_object_struct** board;
    int i_x,i_y;
    int x=0,y=0;
    char buff[500],ch;
    FILE* fp=fopen("board.txt","r");
    fgets(buff,sizeof(buff),fp);
    sscanf(buff,"%d %d",&x,&y);
    board_data.board_size[0]=x;
    board_data.board_size[1]=y;
    board_size[0]=x;
    board_size[1]=y;
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
            if(nbytes<=0)
                return -1;
            Nbytes=Nbytes+nbytes;
    }
    //Now send the score vector
    nbytes=send(client_fd,game_state.scores,sizeof(int)*MAXPLAYERS,0);
    Nbytes=Nbytes+nbytes;

    return Nbytes;

}

int send_initial_message(int client_fd,int player_num){

    /* 
    Initial message protocol:´
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
    printf("[Client setup] Server sent %d bytes do client %d (1/2)\n",nbytes,player_num);
    Nbytes=Nbytes+nbytes;
    //Send seconde part of the message:
    nbytes=send_game_state(client_fd);
    printf("[ClientSetup] Server sent %d bytes to client %d (2/2)\n ",nbytes,player_num);
    Nbytes=Nbytes+nbytes;
    printf("[ClientSetup] Server sent a total of %d bytes to client on startup\n",Nbytes);
    //Message has been sent. 
    return Nbytes;
}


int init_server(){
    struct sockaddr_in server_local_addr;
    int server_socket;
    server_socket=socket(AF_INET,SOCK_STREAM,0);
    if(server_socket==-1){
        perror("socket:");
        exit(-1);
    }
    server_local_addr.sin_family=AF_INET;
    server_local_addr.sin_addr.s_addr=INADDR_ANY;
    server_local_addr.sin_port=htons(DEFAULT_SERVER_PORT);
    int err=bind(server_socket,(struct sockaddr*)&server_local_addr,
                sizeof(server_local_addr));
    if(err==-1){
        perror("bind:");
        exit(-1);
    }

    // should we do the listen here? idont think so
    return server_socket;
}
//this function updates the board after a client request
/* 
    Movement Rules:
    0)If the following block is empt, just move to it, i.e., switch places with it
    1) If there is a brick in the way, bounce back
    2) If two objects of the same player collide, they switch places
    3) If two pacmen, or two monsters, collide, they change places
    4) If a superpacman collides into a monster, it is eaten and put in a different position. The score is incremented
    5) If a monster collides into a pacman, the pacman gets eaten and is put in a different position. The score is incremented
    6) If a pacman and a fruit collide, it eats the fruit and becomes a superpacman
    7) If a monster and a fruit collide, the fruit just disappears
    
*/

// Updates the board, given a message from the client, and returns the amount of movement tokens used
int update_board(int player_num,C2S_message msg){
    int* pos,*next_pos;
    int ret=0;//amount of movement tokens used
    //find matching pacman or moster and update its position
    pos=find_object(player_num,msg.type,board,board_size[0],board_size[1]);
    

    if(pos[0]==-1 || pos[1]==-1){//if object is not found, exit function
        free(pos);
        return 0 ;
    }
    next_pos=calloc(2,sizeof(int));
    next_pos[0]=msg.x;
    next_pos[1]=msg.y;
    //Convert pacman/monster desired position to a position next to the pacman
    closest_square(pos[0],pos[1],next_pos,board_size[0],board_size[1],board);

    


    //Rule 1-Bounce. After applying rule1 the other rules still apply
    if(board[next_pos[1]][next_pos[0]].type==BRICK){
        ret=bounce_back(pos,next_pos,board,board_size[0],board_size[1]);
    }

    //Rule 0- If spot is empty, move to it
    if(is_empty(next_pos[0],next_pos[1],board)){
        switch_places(pos,next_pos,board);
        free(pos);
        free(next_pos);
        return 1;
    }

    //Save player and type, to make the code easier to read =)
    int player1=board[pos[1]][pos[0]].player;
    int player2=board[next_pos[1]][next_pos[0]].player;
    int type1=board[pos[1]][pos[0]].type;//object that wants to move
    int type2=board[next_pos[1]][next_pos[0]].type;//object that is in the way, potentially

    //Rule 2-objects of the same player change positions  
    if(player1==player2&&player1>=0){
        switch_places(pos,next_pos,board);
        ret=1;
    }

    //Rule 3-two pacmen or two monster collide-> they switch places
    
    else if((type1==type2)&&(type1==PACMAN||type1==MONSTER||type1==SUPERPACMAN)){//both are pacmen, superpcamen or monsters
        switch_places(pos,next_pos,board);
        ret=1;
        
    }
  
    //Rule 4 and 5- Monster eats pacman and superpacman eats monster
    else if(type1==MONSTER&&type2==PACMAN){
        eat(pos,next_pos,board);
        game_state.scores[player1]++;
        init_player_position(player2,1,0);
        ret=1;
    }
    else if(type1==PACMAN&&type2==MONSTER){
        eat(next_pos,pos,board);
        game_state.scores[player2]++;
        init_player_position(player1,1,0);
        ret=1;
    }
    else if(type1==SUPERPACMAN&&type2==MONSTER){
        eat(pos,next_pos,board);
        game_state.scores[player1]++;
        init_player_position(player2,0,1);
        ret=1;
    }
    else if(type1==MONSTER&&type2==SUPERPACMAN){
        eat(next_pos,pos,board);
        game_state.scores[player2]++;
        init_player_position(player1,0,1);
        ret=1;
    }

    // Rule 6 and 7 - Eat cherry
    else if(type1==PACMAN&&type2==CHERRY){
        eat(pos,next_pos,board);
        board[next_pos[1]][next_pos[0]].type=SUPERPACMAN; 
        ret=1;
    }
    else if(type2==CHERRY&&(type1==MONSTER||type1==SUPERPACMAN)){
        eat(pos,next_pos,board);
        ret=1;
    }

// End of movement rules

    free(pos);
    free(next_pos);
    return ret;
}

void* token_refill_thread(void*arg){
    printf("Entered\n");
    token_data_struct* token_data= (token_data_struct*)arg;
    time_t* t0=token_data->t0;
    time_t* tf=token_data->tf;
    int* tokens = token_data->move_tokens;
    time(t0);
    while(1){
        time(tf);
        if(difftime(*tf,*t0)>=1 && *tokens<2){
            *tokens=TOKENS_PER_SECOND;
            *t0=*tf;
        }
    }


}



// Thread that receives updates from each client and triggers an event (?)
void* client_thread(void* client_args){
    pthread_t token_refill_thread_id;
    time_t t0,tf; //to store last time the tokens were refilled
    int move_tokens=2;//player receives 2 tokens per second
    client_thread_args args = *(client_thread_args*)client_args;
    int client_fd=args.fd;
    int player_num=args.player_num;
    
    int success;
    success = args.success;
    send(client_fd,&success, sizeof(success),0);

    if(success == 0)
        return NULL;

    init_player_position(player_num,1,1);//initiate player position(player_num,do_player,do_monster)
    printf("Sending initial message to player %d\n",player_num);
    send_initial_message(client_fd,player_num); 
    client_fd_list[player_num]=client_fd;
    int err_rcv;
    C2S_message msg;
    //call thread to refill tokens
    token_data_struct token_args;
    token_args.move_tokens=&move_tokens;
    token_args.t0=&t0;
    token_args.tf=&tf;
    pthread_create(&token_refill_thread_id,NULL,token_refill_thread,&token_args);
    int ret;
    while((err_rcv = recv(client_fd_list[player_num],&msg,sizeof(msg),0))>0 ){
        printf("[Client request] Received %d bytes from client %d \n",err_rcv,player_num);
        // handle message from client
        if(move_tokens>0){
            ret=update_board(player_num,msg);
            move_tokens=move_tokens-ret;
        }
        if(difftime(tf,t0)>30){
            init_player_position(player_num,1,1);//make player jump to random position and delete previous positions
            printf("Inactivity jump\n");
            t0=tf;
        }
    }
}

void* accept_thread(void* arg){ 
    int server_socket = *(int*)arg;

    client_thread_args client_data;
    struct sockaddr_in client_addr;
    socklen_t size_addr = sizeof(client_addr);
    pthread_t client_thread_ids[MAXPLAYERS];
    int client_fd;

    if(listen(server_socket,MAXPLAYERS)==-1){
        perror("listen");
        exit(-1);
    }
    //printf("Waiting for connections\n");

    while(1){
        int i=0;

        printf("[Accept thread] Ready to accept a new connection\n");

        while(client_fd_list[i] !=0)
            i++;

        printf("Socket = %d\n", server_socket);
        client_fd = accept(server_socket,(struct sockaddr*)&client_addr,&size_addr);

        if(client_fd==-1){
            perror("accept:");
            exit(-1);
        }
        
        //Checks if max number is reached
        if(player_connections >= MAXPLAYERS)
            client_data.success = 0;
        else{
            client_data.success = 1;
            player_connections++;
        }

        printf("Accepted new client (id=%d) from %d\n",i,client_fd);
  
        client_data.fd=client_fd;
        client_data.player_num = i;      
        pthread_create(&client_thread_ids[i],NULL,client_thread,&client_data);

        usleep(1000);
    }

}





