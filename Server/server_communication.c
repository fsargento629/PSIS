#include "server_communication.h"
#include "board_handling.h"
#include "server.h"
// sends the board to a certain client
int send_game_state(int client_fd,game_object_struct* vector,int size){
    int nbytes,Nbytes=0;
    int i;
    //Send message size
    int msg_size=size;
    nbytes=send(client_fd,&msg_size,sizeof(int),0);
    Nbytes=nbytes;

    //Now, send the vector
    nbytes=send(client_fd,vector,size*sizeof(game_object_struct),0);
    Nbytes=Nbytes+nbytes;
   
    return Nbytes;

}

// sends the first message to the client
int send_initial_message(int client_fd,int player_num){

    /* 
    Initial message protocol:´
    1) Send player_num and board size to client
    2) Send board to client, line by line by calling the function send_game_state
    3) receive player colors (2 chars)
    
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
    //Send seconde part of the message: board 
    int i;
    pthread_mutex_lock(&board_lock);
    for(i=0;i<board_size[1];i++){
        nbytes=send(client_fd,board[i],board_size[0]*sizeof(game_object_struct),0);
        Nbytes=Nbytes+nbytes;
    }
    pthread_mutex_unlock(&board_lock);
    printf("[ClientSetup] Server sent %d bytes to client %d (2/2)\n ",nbytes,player_num);
    Nbytes=Nbytes+nbytes;
    printf("[ClientSetup] Server sent a total of %d bytes to client on startup\n",Nbytes);
    //Message has been sent. 
    return Nbytes;
}

//initializes server at a given port
int init_server(int port){
    struct sockaddr_in server_local_addr;
    int server_socket;
    server_socket=socket(AF_INET,SOCK_STREAM,0);
    if(server_socket==-1){
        perror("socket:");
        exit(-1);
    }
    server_local_addr.sin_family=AF_INET;
    server_local_addr.sin_addr.s_addr=INADDR_ANY;
    server_local_addr.sin_port=htons(port);
    int err=bind(server_socket,(struct sockaddr*)&server_local_addr,
                sizeof(server_local_addr));
    if(err==-1){
        perror("bind:");
        exit(-1);
    }

    // should we do the listen here? idont think so
    return server_socket;
}

/* Thread that communicates with client and calls the update board function
when needed */
void* client_thread(void* client_args){

    struct timeval t0_pacman; //to store last time the tokens were refilled
    struct timeval tf_pacman;
    struct timeval t0_monster; //to store last time the tokens were refilled
    struct timeval tf_monster;

    client_thread_args args = *(client_thread_args*)client_args;
    int client_fd=args.fd;
    int player_num=args.player_num;
    char pacman_color,monster_color;
    int success;
    success = args.success;
    send(client_fd,&success, sizeof(success),0);

    if(success == 0)
        return NULL;


    recv(client_fd,&pacman_color,sizeof(char),0);
    recv(client_fd,&monster_color,sizeof(char),0);

    printf("Player %d colors: %c %c\n",player_num,pacman_color,monster_color);
    pthread_mutex_lock(&board_lock);
    init_player_position(player_num,1,1,pacman_color,monster_color);//initiate player position(player_num,do_player,do_monster)
    pthread_mutex_unlock(&board_lock);

    printf("Sending initial message to player %d\n",player_num);
    send_initial_message(client_fd,player_num); 
    client_fd_list[player_num]=client_fd;
    int err_rcv;


    C2S_message msg;

    int ret;

    //Initialise times
    gettimeofday(&t0_pacman, NULL);
    gettimeofday(&t0_monster, NULL); 


    do{
        err_rcv = recv(client_fd_list[player_num],&msg,sizeof(msg),0);
        gettimeofday(&tf_pacman, NULL);
        gettimeofday(&tf_monster, NULL);

        //printf("[Client request] Received %d bytes from client %d \n",err_rcv,player_num);
        // handle message from client
        if(time_delta(&tf_pacman, &t0_pacman) >= TOKEN_COOLDOWN && (msg.type == PACMAN || msg.type == SUPERPACMAN)){
            ret = update_board(player_num,msg);
            if(ret == 1)
                gettimeofday(&t0_pacman, NULL);
        }

         if(time_delta(&tf_monster, &t0_monster) >= TOKEN_COOLDOWN && msg.type==MONSTER){
             ret = update_board(player_num,msg);
            if(ret == 1)
                gettimeofday(&t0_monster, NULL);
        }

        if(time_delta(&tf_pacman,&t0_pacman)>=INACTIVITY_TIME&&time_delta(&tf_monster,&t0_monster)>=INACTIVITY_TIME){
            init_player_position(player_num,1,1,pacman_color,monster_color);//make player jump to random position and delete previous positions
            printf("Inactivity jump\n");
            gettimeofday(&t0_pacman,NULL);
            gettimeofday(&t0_monster,NULL);
        }
    }while(client_fd_list[player_num]!=0&&err_rcv>0);

}


//Thread that accepts new connections
void* accept_thread(void* arg){ 
    int server_socket = *(int*)arg;
    client_thread_args client_data;
    struct sockaddr_in client_addr;
    socklen_t size_addr = sizeof(client_addr);
    pthread_t* client_thread_ids=calloc(maxplayers,sizeof(pthread_t));
    int client_fd;


    if(listen(server_socket,MAX_CONNECTIONS)==-1){
        perror("listen");
        exit(-1);
    }
    //printf("Waiting for connections\n");
    
    while(1){
        

        printf("[Accept thread] Ready to accept a new connection\n");

        client_fd = accept(server_socket,(struct sockaddr*)&client_addr,&size_addr);

        if(client_fd==-1){
            perror("accept:");
            exit(-1);
        }
        
        //Checks if max number is reached
        if(player_connections >= maxplayers)
            client_data.success = 0;
        else{
            client_data.success = 1;
            player_connections++;
        }
        int i=0;
        while(client_fd_list[i] !=0)
            i++;
        printf("Accepted new client (id=%d) from %d\n",i,client_fd);
  
        client_data.fd=client_fd;
        client_data.player_num = i;      
        pthread_create(&client_thread_ids[i],NULL,client_thread,&client_data);

        usleep(ACCEPT_THREAD_SLEEP*1000);
    }

}


// thread that sends the score to a given client
void* send_score_thread(void* arg){
    int* score_fd=(int*)arg;
    int nbytes;
    time_t t0,tf;
    t0=time(NULL);
    nbytes=send(*score_fd,scores,(maxplayers)*sizeof(int),0);
    while (nbytes>0&&*score_fd!=0){
        tf=time(NULL);
        if(difftime(tf,t0)>=SCORE_THREAD_COOLDOWN){
            nbytes=send(*score_fd,scores,(maxplayers)*sizeof(int),0);
            t0=time(NULL);
        }
        usleep(10*1000);
    } 
    
    

}


//thread that accepts new connections to send the score
void* accept_score_thread(void* arg){

    int max_players=*(int*)arg;
    client_score_fd=calloc(max_players,sizeof(int));
    struct sockaddr_in client_addr;
    socklen_t size_addr = sizeof(client_addr);
    pthread_t* score_thread_ids=calloc(max_players,sizeof(pthread_t));
    //initiaize server
    int score_socket=init_server(DEFAULT_SCORE_SERVER_PORT);
    //score_socket test is done by init server

    if(listen(score_socket,MAX_CONNECTIONS)==-1){
        perror("listen");
        exit(-1);
    }


    int i=0;
    while(1){
        int aux=accept(score_socket,(struct sockaddr*)&client_addr,&size_addr);
        if(aux==-1){
            perror("Score accept:");
            exit(-1);
        }
        i=0;
        while(client_score_fd[i]!=0)
            i++;
        client_score_fd[i]=aux;
        //create new send_score_thread for client i
        scores[i]=0;
        pthread_create(&(score_thread_ids[i]),NULL,send_score_thread,&(client_score_fd[i]));

    }
    

    
    for(i=0;i<max_players;i++)
        pthread_join(score_thread_ids[i],NULL);
    free(client_score_fd);
    free(score_thread_ids);
    
}


//handles the disconnect protocol
void disconnect(int player_id){
    
    client_fd_list[player_id]=0;
    client_score_fd[player_id]=0;
    scores[player_id]=-1;
    player_connections--;

    //clear player from board:
    int x,y;
    for(y=0;y<board_size[1];y++){
        for(x=0;x<board_size[0];x++){
            if(board[y][x].player==player_id){
                clear_board_cell(x,y,board);
            }
        }
    }

}


// stores the moving elements in board to a vector and returns the size of he vector
vector_struct board2vector(){
    int x,y;
    int size;
    int i;
    vector_struct vector;
    game_object_struct* aux_vector= calloc(board_size[0]*board_size[1],sizeof(game_object_struct));
    i=0;
    for(y=0;y<board_size[1];y++){
        for(x=0;x<board_size[0];x++){
            if(board[y][x].type!=EMPTY && board[y][x].type!=BRICK ){
                aux_vector[i]=board[y][x];
                aux_vector[i].x=x;
                aux_vector[i].y=y;//make sure that the position is correct
                i++;
            }
        }
    }
    size=i;
    vector.size=size;
    if(size>0){
        vector.data=(game_object_struct*) calloc(size,sizeof(game_object_struct));
        for(i=0;i<size;i++){
            vector.data[i]=aux_vector[i];
        }
    }
    else
    {
        vector.data=NULL;
    }
    
    free(aux_vector);
    return vector;
   
}

void signal_callback_handler(int signum){
}
