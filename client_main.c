#include <SDL2/SDL.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include "client.h"
#include "client_communication.h"
#include "drawing.h"

int main(int argc , char* argv[]){
	if(argc!=5){
        printf("Program must be run as:\n./client <SERVER_IP> >SERVER_PORT> <pacman_color> <monster_color>\n");
        exit(-1);
    }
    signal(SIGINT, signal_kill_handler);
	char pacman_color=*argv[3];
	char monster_color=*argv[4];	
	int *pos;
	int nbytes;
    Uint32 Event_screen_refresh;
	SDL_Event event;
	int done = 0;
	int x,y,x_new,y_new;;
	pthread_t sock_thread_ID,score_thread_id;
	Event_screen_refresh =  SDL_RegisterEvents(1);
	board_struct* old_board,*new_board;
	old_board=malloc(sizeof(board_struct));
	new_board=malloc(sizeof(board_struct));
	
    //setup of communication
	nbytes=setup_comm(argv[1],argv[2],new_board,&pacman_color,&monster_color);

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
	score_thread_id=pthread_create(&score_thread_id,NULL,receive_score_thread,argv[1]);
	create_board_window(board_size[0],board_size[1]);
	update_screen(NULL,new_board->board,1);//draw bricks
	old_board=new_board;
	int i=0;
	int pacman_or_superpacman=PACMAN;
	printf("Initiating main loop\n");
    while(!done){

        while (SDL_PollEvent(&event)) {
			usleep(CLIENT_SLEEP*1000);
			if(event.type == SDL_QUIT) {
					done = SDL_TRUE;
			}
            if(event.type==Event_screen_refresh && screen_ready==1){//server has sent a message
				screen_ready=0;
				new_board= event.user.data1;
				pos=find_object(player_id,PACMAN,old_board->board,board_size[0],board_size[1]);
				if(pos[0]==OBJECT_NOT_FOUND){//if cant find a pacman, it mjust be a superpacman
					pos=find_object(player_id,SUPERPACMAN,old_board->board,board_size[0],board_size[1]);
					pacman_or_superpacman=SUPERPACMAN;
				}
				else
				{
					pacman_or_superpacman=PACMAN;
				}
				
                update_screen(old_board->board,new_board->board,0); 
				//printf("Screen updated\n");    
				
				//free_board(old)
				//printf("freed game_state\n");
              	old_board=new_board;
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


			if(event.type==SDL_KEYDOWN){
				if(event.key.keysym.sym==SDLK_LEFT){
					//left key
					pos=find_object(player_id,MONSTER,old_board->board,board_size[0],board_size[1]);
					if(pos[0]!=-1&&pos[1]!=-1)//not found
						nbytes=send_move(pos[0]-1,pos[1],MONSTER);
					free(pos);
				}
				if(event.key.keysym.sym==SDLK_RIGHT){
					//right key
					pos=find_object(player_id,MONSTER,old_board->board,board_size[0],board_size[1]);
					if(pos[0]!=-1&&pos[1]!=-1)//not found
						nbytes=send_move(pos[0]+1,pos[1],MONSTER);
					free(pos);
				}
				if(event.key.keysym.sym==SDLK_UP){
					//up key
					pos=find_object(player_id,MONSTER,old_board->board,board_size[0],board_size[1]);
					if(pos[0]!=-1&&pos[1]!=-1)//not found
						nbytes=send_move(pos[0],pos[1]-1,MONSTER);
					free(pos);
				}
				if(event.key.keysym.sym==SDLK_DOWN){
					//down key
					pos=find_object(player_id,MONSTER,old_board->board,board_size[0],board_size[1]);
					if(pos[0]!=-1&&pos[1]!=-1)// found a match
						nbytes=send_move(pos[0],pos[1]+1,MONSTER);
					free(pos);
				}
			}						
		}
    }
    printf("\n\nFim\n");
    exit(0);
}
