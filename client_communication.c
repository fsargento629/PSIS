#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include<unistd.h>
#include "client_communication.h"
#include "drawing.h"

//receives the board from the server
int receive_initial_game_state(board_struct* new_board,int socket_fd){
    int nbytes,Nbytes=0;
    int i=0;
    game_object_struct** board=(game_object_struct**)malloc(sizeof(game_object_struct*) * board_size[1]); 
    //printf("[Setup] Receiving board from server\n");
    for(i=0;i<board_size[1];i++){
        board[i] =(game_object_struct*) malloc (sizeof(game_object_struct) * board_size[0]);
        //receive line from server
        nbytes=recv(sock_fd,board[i],sizeof(game_object_struct) * board_size[0],0);
        Nbytes=Nbytes+nbytes;
        
    }
    
    new_board->board=board;
    new_board->size_x=board_size[0];
    new_board->size_y=board_size[1];
    return Nbytes;
}

/* 
    Initial message protocol:
    1) Receive if it was denied or accepted by the server
    2) Receive player_num and board size 
    3) Receive board , line by line by calling the function receive_game_state
    
    */

//handles the client setup protocol
int setup_comm(char* server_ip,char* port,board_struct* new_board,char* pacman_color,char* monster_color){
    int nbytes,Nbytes=0;
    setup_message msg;
    struct sockaddr_in server_addr;
	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_fd == -1){
		perror("socket: ");
		exit(-1);
    }


	server_addr.sin_family = AF_INET;
	int port_number;
	if(sscanf(port, "%d", &port_number)!=1){
		printf("argv[2] is not a number\n");
		exit(-1);
	}
	  server_addr.sin_port= htons(port_number);
	  if(inet_aton(server_ip, &server_addr.sin_addr) == 0){
			printf("argv[1]is not a valida address\n");
			exit(-1);
		}

	printf("connecting to %s on %d\n", server_ip, server_addr.sin_port);

	if(connect(sock_fd,(const struct sockaddr*)&server_addr,
                                    sizeof(server_addr))==-1){
        printf("Error connecting to server\n");
        exit(-1);                               
    }
    
    //check if server is full
    int success;
    nbytes = recv(sock_fd ,&success , sizeof(success),0); 
    
    if (success == 0)
    {
        printf("Server is full\n");
        close(sock_fd);
        exit(-1);
    }

    //Send colors:
    nbytes=send(sock_fd,pacman_color,sizeof(char),0);
    if(nbytes<=0){
        printf("Unable to send pacman color\n");
        exit(-1);
    }
    nbytes=send(sock_fd,monster_color,sizeof(char),0);
    if(nbytes<=0){
        printf("Unable to send monster color\n");
        exit(-1);
    }
    printf("Colors sent to server\n");
    //Initiating setup protocol
    nbytes = recv(sock_fd ,&msg , sizeof(setup_message),0); 
    printf("[Setup] Received %d bytes from server (1/2)\n",nbytes);
    Nbytes=Nbytes+nbytes;
    player_id=msg.player_num;
    board_size[0]=msg.board_size[0];
    board_size[1]=msg.board_size[1];
    max_players=msg.max_players;
    // Now receiving state:
    nbytes=receive_initial_game_state(new_board,sock_fd);
    printf("[Setup] Received %d bytes from server (2/2)\n",nbytes);
    Nbytes=Nbytes+nbytes;

    return Nbytes;    
}


// Thread to receive board updates from server
void* sock_thread(void* args_pt){
    socket_thread_args* arg= (socket_thread_args*)args_pt;
    int socket_fd=arg->sock_fd;
    int nbytes;
    int vector_size=0;
    game_object_struct* vector_data;
    SDL_Event new_event;
    vector_struct* vector;
    //loop receiving messages from the server and refreshing main thread 
    while(1){
        usleep(SOCKTHREAD_USLEEP*1000);
        //printf("Entering receive_game_state\n");
        
        //first, receive the size of the msg
        nbytes=recv(socket_fd,&vector_size,sizeof(int),0);
        if(nbytes <= 0)//disconnect
        {
            printf("Server shut down. Closing client\n");
            close(sock_fd);
            exit(0);
        }
        //second, receive the vector
        vector_data=calloc(vector_size,sizeof(game_object_struct));
        nbytes=recv(socket_fd,vector_data,vector_size*sizeof(game_object_struct),0);
        vector=malloc(sizeof(vector_struct));
        vector->data=vector_data;
        vector->size=vector_size;
        //printf("[Socket thread] Received %d bytes from server\n",nbytes);
        SDL_zero(new_event);
        new_event.type = arg->Event_screen_refresh;
        new_event.user.data1=vector;  

        SDL_PushEvent(&new_event); 
    }    
}


// Sends a move to the server
int send_move(int x,int y,int type){
    C2S_message msg;
    int nbytes;
    msg.x=x;
    msg.y=y;
    msg.type=type;
    nbytes=send(sock_fd,&msg,sizeof(C2S_message),0);
    return nbytes;
}

//receives the score from the server and prints it
void* receive_score_thread(void*arg){
    char* server_ip= (char*)arg;
    int score_socket;
    int nbytes=0;
    int* score=calloc(max_players,sizeof(int));
    //setup score socket
    struct sockaddr_in server_addr;
    score_socket=socket(AF_INET,SOCK_STREAM,0);
    if(score_socket==-1){
        perror("socket: ");
        exit(-1);}

    server_addr.sin_family=AF_INET;
    server_addr.sin_port=htons(DEFAULT_SCORE_SERVER_PORT);
    inet_aton(server_ip, &server_addr.sin_addr);
    if( -1 == connect(score_socket,
			(const struct sockaddr *) &server_addr,
			sizeof(server_addr))){
				printf("Error connecting to server score socket\n");
				exit(-1);
	}


    //loop receiving messages from the server and refreshing main thread 
   do{
        nbytes=recv(score_socket,score,max_players*sizeof(int),0);
        print_score_board(score,max_players);
        usleep(SCORE_THREAD_SLEEP*1000);
    }while(nbytes>=0);   
    printf("[Receive score thread] Server shut down. Closing client\n");
    close(score_socket);
    free(score);
}


//order the score using bubble sort and print it
void print_score_board(int* score,int size_score){

    int i=0,j=0;
    int aux;
    int* player_ids=calloc(size_score,sizeof(int));

    //intiialize player_ids
    for(i=0;i<size_score;i++)
        player_ids[i]=i;
    
    //order the score vector and save the changes in the player_ids vector
    for(i=0;i<size_score;i++){
        for(j=0;j<size_score-i-1;j++){
            if(score[j+1]>score[j]){
                //swap player ids
                aux=player_ids[j+1];
                player_ids[j+1]=player_ids[j];
                player_ids[j]=aux;
                //swap scores
                aux=score[j+1];
                score[j+1]=score[j];
                score[j]=aux;
            }
        }
    }



    //now print it:
    printf("---------------------\n");
    printf("----Score board----\n");
    for(i=0;i<size_score;i++){
        if(score[i]==-1)
            break;
        printf("%d)-> Player %d -> %d points\n",i+1,player_ids[i],score[i]);
    }
    printf("---------------------\n");
    free(player_ids);
}



//send monster request to server
int move_monster(SDL_Keycode keycode,vector_struct vector){
    int nbytes;
    int x,y;
    find_object_in_vector(vector,MONSTER,player_id,&x,&y);
    if(keycode==SDLK_LEFT){
        //left key
        if (x != -1 && y != -1) //not found
            nbytes = send_move(x - 1, y, MONSTER);
        
    }
    else if (keycode == SDLK_RIGHT)
    {
        //right key
        if (x != -1 && y != -1) //not found
            nbytes = send_move(x + 1, y, MONSTER);
        
    }
    else if (keycode == SDLK_UP)
    {
        //up key
        if (x != -1 && y != -1) //not found
            nbytes = send_move(x, y - 1, MONSTER);
    }
    else if (keycode == SDLK_DOWN)
    {
        //down key
        if (x != -1 && y != -1) // found a match
            nbytes = send_move(x, y + 1, MONSTER);
    }
    return nbytes;
}

void client_signal_kill_handler(int signum){
    printf("\nShutting down due to ctrl+C\n");
    exit(0);
}
