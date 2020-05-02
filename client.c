#include <SDL2/SDL.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include<unistd.h>
#include "UI/UI_library.h"
#include "Server/common.h"
#include "client.h"

//Initial message received by server
//Returns struct of setup message

setup_message setup_comm(char* server_ip,int* sock_fd,char* port,
                                struct sockaddr_in* server_addr){

    
    setup_message first_message;
	*sock_fd= socket(AF_INET, SOCK_STREAM, 0);
	if (*sock_fd == -1){
		perror("socket: ");
		exit(-1);
    }

	server_addr->sin_family = AF_INET;
	int port_number;
	if(sscanf(port, "%d", &port_number)!=1){
		printf("argv[2] is not a number\n");
		exit(-1);
	}
	  server_addr->sin_port= htons(port_number);
	  if(inet_aton(server_ip, &server_addr->sin_addr) == 0){
			printf("argv[1]is not a valida address\n");
			exit(-1);
		}

	  printf("connecting to %s %d\n", server_ip, server_addr->sin_port );

	if( -1 == connect(*sock_fd,
	  			        (const struct sockaddr *) server_addr,	sizeof(&server_addr))){
	  				printf("Error connecting\n");
	                exit(-1);
	}
    int nbytes = read(*sock_fd ,&first_message , sizeof(first_message)); 
    printf("Received %d bytes from the server on setup\n",nbytes);
    return first_message;    

}



// This function handles the messages sent from the server to the client
void* sock_thread(void* args_pt){
    socket_thread_args* arg= (socket_thread_args*)args_pt;

    int* sock_fd=arg->sock_fd_pt;
    //game_state_struct* new_game_state=arg->new_game_state_pt;
    S2C_message msg;
    int err_rcv;
    SDL_Event new_event;
    game_state_struct* new_game_state;


    //loop receiving messages from the server and refreshing main thread 
    while((err_rcv=recv(*sock_fd,&msg,sizeof(msg),0))>0){
        printf("Received %d bytes from server\n",err_rcv);
        free(new_game_state);
        new_game_state=malloc(sizeof(game_state_struct));
        *new_game_state=msg.game_state;
        SDL_zero(new_event);
        new_event.type = arg->Event_screen_refresh;
        new_event.user.data1=new_game_state;
        SDL_PushEvent(&new_event);

    }
    printf("Sock thread exiting\n");
  
    
}
//Function to inform the server of a move made by the player
void inform_server(game_object_struct game_object,int sock_fd){
    C2S_message message;

    message.type=game_object.type;
    message.x=game_object.pos[0];
    message.y=game_object.pos[1];
    send(sock_fd,&message,sizeof(message),0);

}

// This function compares the old and new game_states and decides what to draw
void update_screen(game_state_struct* game_state,game_state_struct* new_game_state){

    int i=0;
        //clear and draw in other place if entry has changed:
        for(i=0;i<MAXOBJECTS;i++){
        if(object_changed(game_state->objects[i],new_game_state->objects[i])==1){
            clear_place(game_state->objects[i].pos[0],game_state->objects[i].pos[1]);

            //draw new object
            draw_object(new_game_state->objects[i]);

        }
        
    }               
                        
}

// check if data entry has changed
int object_changed(game_object_struct old,game_object_struct new){
    if(old.pos[0]!=new.pos[0])
        return 1;
    if(old.pos[1]!=new.pos[1])
        return 1;
    if(old.type!=new.type)
        return 1;
    if(old.color!=new.color)
        return 1;
    return 0;
    
}

//draw an object
void draw_object(game_object_struct object){
    if(object.type==0)//empty
        exit(0);
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
