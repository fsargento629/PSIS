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
	if(argc!=5){
        printf("Program must be run as:\n./client <SERVER_IP> >SERVER_PORT> <pacman_color> <monster_color>\n");
        exit(-1);
    }
	char pacman_color=*argv[3];
	char monster_color=*argv[4];	
	int *pos;
	int nbytes;
    Uint32 Event_screen_refresh;
	SDL_Event event;
	int done = 0;
	int x,y,x_new,y_new;;
	pthread_t sock_thread_ID;
	Event_screen_refresh =  SDL_RegisterEvents(1);
	game_state_struct* game_state,* new_game_state;
	game_state=malloc(sizeof(game_state_struct));
	new_game_state=malloc(sizeof(game_state_struct));
	signal(SIGINT, signal_kill_handler);
    //setup of communication
	nbytes=setup_comm(argv[1],argv[2],game_state,&pacman_color,&monster_color);
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
	int pacman_or_superpacman=PACMAN;
    while(!done){

        while (SDL_PollEvent(&event)) {
			usleep(CLIENT_SLEEP*1000);
			if(event.type == SDL_QUIT) {
					done = SDL_TRUE;
			}
            if(event.type==Event_screen_refresh && screen_ready==1){//server has sent a message
				screen_ready=0;
                new_game_state=event.user.data1;// receives a new game_state struct
				pos=find_object(player_id,PACMAN,game_state->board,board_size[0],board_size[1]);
				if(pos[0]==OBJECT_NOT_FOUND){//if cant find a pacman, it mjust be a superpacman
					pos=find_object(player_id,SUPERPACMAN,game_state->board,board_size[0],board_size[1]);
					pacman_or_superpacman=SUPERPACMAN;
				}
				else
				{
					pacman_or_superpacman=PACMAN;
				}
				
				//x=pos[0];
				//y=pos[1];
				
                update_screen(game_state->board,new_game_state->board,0); 
				//printf("Screen updated\n");
				//free(game_state->board);//delete old board        
				
				free(game_state);
				//printf("freed game_state\n");
              	game_state=new_game_state;
				screen_ready=1;
				//send move to server, if pacman position is different than mouse position
				SDL_GetMouseState(&x,&y);
            	get_board_place(x, y, &x_new, &y_new);
				if (x_new!=pos[0]||y_new!=pos[1])
				{
					//send pacman/superpacman move request
					nbytes=send_move(x_new,y_new,pacman_or_superpacman);

				}
				
				free(pos);

            }

            /*if(event.type==SDL_MOUSEMOTION){
                //Send info about pacman to server
				
				get_board_place(event.motion.x,event.motion.y,&x_new,&y_new);
				//if the mouse is different, send to server
				if(x!=x_new || y!=y_new){
					//printf("[[Move request]... ");
					printf("x=%d|y=%d\n",x_new,y_new);
					nbytes=send_move(x_new,y_new,pacman_or_superpacman);//send move request to server
					//printf("Sent %d bytes to server\n",nbytes);
					
				}
		
            }*/

			if(event.type==SDL_KEYDOWN){
				if(event.key.keysym.sym==SDLK_LEFT){
					//left key
					pos=find_object(player_id,MONSTER,game_state->board,board_size[0],board_size[1]);
					if(pos[0]!=-1&&pos[1]!=-1)//not found
						nbytes=send_move(pos[0]-1,pos[1],MONSTER);
					printf("x_m=%d|y_m=%d\n",pos[0]-1,pos[1]);
					free(pos);
				}
				if(event.key.keysym.sym==SDLK_RIGHT){
					//right key
					pos=find_object(player_id,MONSTER,game_state->board,board_size[0],board_size[1]);
					if(pos[0]!=-1&&pos[1]!=-1)//not found
						nbytes=send_move(pos[0]+1,pos[1],MONSTER);
					printf("x_m=%d|y_m=%d\n",pos[0]+1,pos[1]);
					free(pos);
				}
				if(event.key.keysym.sym==SDLK_UP){
					//up key
					pos=find_object(player_id,MONSTER,game_state->board,board_size[0],board_size[1]);
					if(pos[0]!=-1&&pos[1]!=-1)//not found
						nbytes=send_move(pos[0],pos[1]-1,MONSTER);
					printf("x_m=%d|y_m=%d\n",pos[0],pos[1]-1);
					free(pos);
				}
				if(event.key.keysym.sym==SDLK_DOWN){
					//down key
					pos=find_object(player_id,MONSTER,game_state->board,board_size[0],board_size[1]);
					if(pos[0]!=-1&&pos[1]!=-1)// found a match
						nbytes=send_move(pos[0],pos[1]+1,MONSTER);
					printf("x_m=%d|y_m=%d\n",pos[0],pos[1]+1);
					free(pos);
				}
			}						
		}
    }
    printf("\n\nFim\n");
    exit(0);
}
