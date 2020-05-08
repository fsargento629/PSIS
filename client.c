#include <SDL2/SDL.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include<unistd.h>
#include "UI_library.h"
#include "Server/common.h"
#include "client.h"


// changes the content of game_state and returns number of bytes read
int receive_game_state(game_state_struct* game_state,int socket_fd){
    int nbytes,Nbytes=0;
    int i=0;
    game_object_struct** board=malloc(sizeof(game_object_struct*)*board_size[1]); 
    //printf("[Setup] Receiving board from server\n");
    for(i=0;i<board_size[1];i++){
        board[i] = malloc (sizeof(game_object_struct) * board_size[0]);
        //receive line from server
        nbytes=recv(sock_fd,board[i],sizeof(game_object_struct)*board_size[0],0);
        Nbytes=Nbytes+nbytes;
        
    }
    //receive score
    int scores[MAXPLAYERS];
    nbytes=recv(sock_fd,scores,sizeof(scores),0);
    Nbytes=Nbytes+nbytes;
    //printf("[Receive game_state] Received %d bytes from server\n",Nbytes);//
    for(i=0;i<MAXPLAYERS;i++)
        game_state->scores[i]=scores[i];
    //initial game state has been received
    game_state->board=board;
    //printf("Exiting receive state\n");
   
    return Nbytes;
}


//Initial message received by server
//Returns number of bytes read

 /* 
    Initial message protocol:
    1) Receive player_num and board size 
    2) Receive board , line by line by calling the function receive_game_state
    3) Receive the scores (Also given by receive_game_state)
    
    */
int setup_comm(char* server_ip,char* port,game_state_struct* game_state){
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

	printf("connecting to %s on %d\n", server_ip, server_addr.sin_port );

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
    

    //Initiating setup protocol
    nbytes = recv(sock_fd ,&msg , sizeof(setup_message),0); 
    printf("[Setup] Received %d bytes from server (1/2)\n",nbytes);
    Nbytes=Nbytes+nbytes;
    player_id=msg.player_num;
    board_size[0]=msg.board_size[0];
    board_size[1]=msg.board_size[1];
    // Now receiving state:
    nbytes=receive_game_state(game_state,sock_fd);
    printf("[Setup] Received %d bytes from server (2/2)\n",nbytes);
    Nbytes=Nbytes+nbytes;

    return Nbytes;    


}



// This function handles the messages sent from the server to the client
// Triggers an event when it receives a new msg (board,score)
// Args-> sock_fd and event id
void* sock_thread(void* args_pt){
    socket_thread_args* arg= (socket_thread_args*)args_pt;
    int socket_fd=arg->sock_fd;
    int nbytes;
    SDL_Event new_event;
    game_state_struct* new_game_state;
    int disconnect=0;

    //loop receiving messages from the server and refreshing main thread 
    while(disconnect==0){
        new_game_state=malloc(sizeof(game_state_struct));//aloocate space for new_game_state
        //printf("Entering receive_game_state\n");
        nbytes=receive_game_state(new_game_state,socket_fd);

        if(nbytes <= 0)//disconnect
        {
            printf("Server shut down. Closing client\n");
            exit(0);
        }

        printf("[Socket thread] Received %d bytes from server\n",nbytes);
        SDL_zero(new_event);
        new_event.type = arg->Event_screen_refresh;
        new_event.user.data1=new_game_state;
        printf("Sent event to main\n");
        SDL_PushEvent(&new_event);
    }    
}


//draw an object
void draw_object(game_object_struct object){
    if(object.type==0)//empty
        clear_place(object.pos[0],object.pos[1]);

    int color[3]={0,0,0};
    color[object.color]=255;

    if(object.type==1)//pacman
        paint_pacman(object.pos[0],object.pos[1],color[0],color[1],color[2]);

    else if(object.type==2)//mosnter
        paint_monster(object.pos[0],object.pos[1],color[0],color[1],color[2]);
    
    else if(object.type==3)//brick
        paint_brick(object.pos[0],object.pos[1]);
    
    else if(object.type==4)//cherry
        paint_cherry(object.pos[0],object.pos[1]);    
    
    else if(object.type==5)//lemon
        paint_lemon(object.pos[0],object.pos[1]);
         
}


int send_move(int x,int y,int type){
    printf("Sending move to server\n");
    C2S_message msg;
    int nbytes;
    msg.x=x;
    msg.y=y;
    msg.type=type;
    nbytes=send(sock_fd,&msg,sizeof(C2S_message),0);
    return nbytes;
}

void update_screen(game_object_struct** old_board,game_object_struct** new_board,int override){
    int x,y;
    //check for differences.
    //If there is a difference,paint it
    
    for(y=0;y<board_size[1];y++){
        for(x=0;x<board_size[0];x++){
            if(override){
                draw_object(new_board[y][x]);
                continue;
            }
        
            if(objects_are_different(old_board[y][x],new_board[y][x]))
                draw_object(new_board[y][x]);
        
        }
    }

    
}
