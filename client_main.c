#include <SDL2/SDL.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "UI_library.h"
#include "Server/common.h"
#include "client.h"
#include <unistd.h>



int main(int argc , char* argv[]){
	int pacman_id,monster_id;
	int nbytes;
    Uint32 Event_screen_refresh;
	SDL_Event event;
	int done = 0;
	int x,y;
	pthread_t sock_thread_ID;
	Event_screen_refresh =  SDL_RegisterEvents(1);
	game_state_struct* game_state,* new_game_state;
   
    //setup of communication
	nbytes=setup_comm(argv[1],argv[2],game_state);
	


    // Create socket thread 
	socket_thread_args args;
	args.sock_fd=sock_fd;
	args.Event_screen_refresh=Event_screen_refresh;
	sock_thread_ID=pthread_create(&sock_thread_ID,NULL,sock_thread,&args);
	printf("Created socket thread\n");

	create_board_window(board_size[0],board_size[1]);
	printf("Created board\n");

	int i=0;
    while(!done){

        while (SDL_PollEvent(&event)) {
			if(event.type == SDL_QUIT) {
					done = SDL_TRUE;
			}
            if(event.type==Event_screen_refresh){//server has sent a message
                new_game_state=(game_state_struct*)(event.user.data1);//allocates and receives a new game_state struct
                update_screen(game_state->board,new_game_state->board);          
              	game_state=new_game_state;
				free(new_game_state);
				usleep(10);
            }

            if(event.type==SDL_MOUSEMOTION){
                //Send info about pacman to server
				int x_new,y_new;
				get_board_place(event.motion.x,event.motion.y,&x_new,&y_new);
				//if the mouse is different, send to server
				if(x!=x_new || y!=y_new){
					nbytes=send_move(x_new,y_new,1);//send move request to server
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
