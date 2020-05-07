#include <SDL2/SDL.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "UI_library.h"
#include "Server/common.h"
#include "client.h"
#include <unistd.h>
#include <signal.h>

int main(int argc , char* argv[]){
	//int pacman_id,monster_id;
	int nbytes;
    Uint32 Event_screen_refresh;
	SDL_Event event;
	int done = 0;
	int x,y;
	pthread_t sock_thread_ID;
	Event_screen_refresh =  SDL_RegisterEvents(1);
	game_state_struct* game_state,* new_game_state;
	game_state=malloc(sizeof(game_state_struct));
	new_game_state=malloc(sizeof(game_state_struct));
	signal(SIGINT, signal_kill_handler);
    //setup of communication
	nbytes=setup_comm(argv[1],argv[2],game_state);
	printf("[Setup] Read %d bytes from server on setup\n",nbytes);
	printf("[Setup] Board size: %dx%d\n",board_size[0],board_size[1]);
	if(nbytes==0){
		printf("Server startup connection error\n");
		exit(-1);
	}
	printf("Setup complete\n");
    // Create socket thread 
	socket_thread_args args;
	int screen_ready=1;
	args.sock_fd=sock_fd;
	args.Event_screen_refresh=Event_screen_refresh;
	sock_thread_ID=pthread_create(&sock_thread_ID,NULL,sock_thread,&args);
	printf("Created socket thread\n");
	create_board_window(board_size[0],board_size[1]);
	printf("Created board\n");
	update_screen(NULL,game_state->board,1);//draw bricks
	//printf_game_state(board_size[0],board_size[1],game_state);
	int i=0;
    while(!done){

        while (SDL_PollEvent(&event)) {
			if(event.type == SDL_QUIT) {
					done = SDL_TRUE;
			}
            if(event.type==Event_screen_refresh && screen_ready==1){//server has sent a message
				screen_ready=0;
                new_game_state=event.user.data1;// receives a new game_state struct
                update_screen(game_state->board,new_game_state->board,0); 
				//printf("Screen updated\n");
				//free(game_state->board);//delete old board        
				
				free(game_state);
				//printf("freed game_state\n");
              	game_state=new_game_state;
				//printf("Updated game_state\n");				
				//usleep(100*1000);
				screen_ready=1;
				//printf("Screen ready\n");
            }

            if(event.type==SDL_MOUSEMOTION){
                //Send info about pacman to server
				int x_new,y_new;
				get_board_place(event.motion.x,event.motion.y,&x_new,&y_new);
				//if the mouse is different, send to server
				if(x!=x_new || y!=y_new){
					printf("[[Move request]... ");
					nbytes=send_move(x_new,y_new,1);//send move request to server
					printf("Sent %d bytes to server\n",nbytes);
					x=x_new;
					y=y_new;
				}
			//do event for monster				
            }
		}
    }
    printf("\n\nFim\n");
    close_board_windows();
    exit(0);
}
