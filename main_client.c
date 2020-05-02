#include <SDL2/SDL.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "UI/UI_library.h"
#include "Server/common.h"
#include "client.h"

int pacman_id,monster_id;
int sock_fd;
struct sockaddr_in server_addr;
setup_message first_message;
game_state_struct game_state;
game_state_struct new_game_state;

int main(int argc , char* argv[]){

	Uint32 Event_screen_refresh;
	SDL_Event event;
	int done = 0,board_size[2];
	pthread_t sock_thread_ID;
	Event_screen_refresh =  SDL_RegisterEvents(1);
		 
	

	//setup of communication
	first_message=setup_comm(argv[1],&sock_fd,argv[2],&server_addr);
	game_state=first_message.game_state;
	new_game_state=game_state;
	board_size[0]=first_message.board_size[0];
	board_size[1]=first_message.board_size[1];
	
 
    // Create socket thread 
	socket_thread_args args;
	args.new_game_state_pt=&new_game_state;
	args.sock_fd_pt=&sock_fd;
	args.new_game_state_pt=Event_screen_refresh;
	sock_thread_ID=pthread_create(&sock_thread_ID,NULL,sock_thread,&args);
	
	//creates a windows and a board with 50x20 cases
	create_board_window(board_size[0],board_size[1]);

	

	while (!done){
		while (SDL_PollEvent(&event)) {
			if(event.type == SDL_QUIT) {
					done = SDL_TRUE;
			}
			if(event.type == Event_screen_refresh){
				printf("new event received\n");
				new_game_state=*(game_state_struct*)(event.user.data1);
				void update_screen(&game_state,&new_game_state);
				game_state=new_game_state;
			}

			
			if(event.type == SDL_MOUSEMOTION){
				
				}

			
			}
		}
	
	printf("Fim\n");
	close_board_windows();
	exit(0);
}
