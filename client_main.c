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
    signal(SIGINT, client_signal_kill_handler);
	char pacman_color=*argv[3];
	char monster_color=*argv[4];	
	int nbytes;
    Uint32 Event_screen_refresh;
	SDL_Event event;
	SDL_Window* window;
	int done = 0;
	int x,y,x_new,y_new;
	int pacman_x,pacman_y;
	int monster_x,monster_y;

	pthread_t sock_thread_ID,score_thread_id;
	Event_screen_refresh =  SDL_RegisterEvents(1);

	// object vectors
	vector_struct old_vector;
	old_vector.size=0;
	old_vector.data=NULL;
	vector_struct new_vector;
	
    //setup of communication
	board_struct raw_board;//board without the players
	nbytes=setup_comm(argv[1],argv[2],&raw_board,&pacman_color,&monster_color);
	


	printf("[Setup] Read %d bytes from server on setup\n",nbytes);
	printf("[Setup] Board size: %dx%d\n",board_size[0],board_size[1]);


	if(nbytes==0){
		printf("Server startup connection error\n");
		exit(-1);
	}

	printf("Setup complete\n");

   
	window = create_board_window(board_size[0],board_size[1]);
	initial_draw(raw_board);

	
	 // Create socket thread 
	socket_thread_args args;
	args.sock_fd=sock_fd;
	args.Event_screen_refresh=Event_screen_refresh;
	sock_thread_ID=pthread_create(&sock_thread_ID,NULL,sock_thread,&args);
	score_thread_id=pthread_create(&score_thread_id,NULL,receive_score_thread,argv[1]);


	int i=0;
	int pacman_or_superpacman=PACMAN;
	printf("Initiating main loop\n");

    while(!done){

        while (SDL_PollEvent(&event)) {
			usleep(CLIENT_SLEEP*1000);
			if(event.type == SDL_QUIT) {
					done = SDL_TRUE;
			}
            if(event.type==Event_screen_refresh){//server has sent a message
				vector_struct* aux_vector= event.user.data1;
				new_vector=*aux_vector;
				free(aux_vector);
				
				update_screen(old_vector,new_vector);
				free(old_vector.data);
				old_vector=new_vector;				


				//send move to server, if pacman position is different than mouse position
				if(find_object_in_vector(old_vector,PACMAN,player_id,&pacman_x,&pacman_y)==0){//must be a superpacman, instead of a pacman
					find_object_in_vector(old_vector,SUPERPACMAN,player_id,&pacman_x,&pacman_y);
					pacman_or_superpacman=SUPERPACMAN;
				}
				else
			
				{
						pacman_or_superpacman=PACMAN;
				}
				
				SDL_GetMouseState(&x,&y);
            	get_board_place(x, y, &x_new, &y_new);
				if ((x_new!=pacman_x||y_new!=pacman_y) && isMouseOnWindow(window))
				{
					//send pacman/superpacman move request
					nbytes=send_move(x_new,y_new,pacman_or_superpacman);

				}
				
				
            }

			
			if(event.type==SDL_KEYDOWN){
				move_monster(event.key.keysym.sym,old_vector);
				
			}						
		}
	}
	pthread_cancel(sock_thread_ID);
	pthread_cancel(score_thread_id);
    printf("\n\nFim\n");
    exit(0);
}
