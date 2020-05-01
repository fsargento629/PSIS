#include <SDL2/SDL.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "UI/UI_library.h"
#include "Server/common.h"
#include "client.h"

Uint32 Event_ShowCharacter;
int sock_fd;


int main(int argc , char* argv[]){

	
	SDL_Event event;
	int done = 0,board_size[2];
	pthread_t sock_thread_ID;

	Event_ShowCharacter =  SDL_RegisterEvents(1);
		 
	struct sockaddr_in server_addr;

	sock_fd= socket(AF_INET, SOCK_STREAM, 0);
	if (sock_fd == -1){
		perror("socket: ");
		exit(-1);
    }

		server_addr.sin_family = AF_INET;
		int port_number;
		if(sscanf(argv[2], "%d", &port_number)!=1){
			printf("argv[2] is not a number\n");
			exit(-1);
		}
	  server_addr.sin_port= htons(port_number);
	  if(inet_aton(argv[1], &server_addr.sin_addr) == 0){
			printf("argv[1]is not a valida address\n");
			exit(-1);
		}

	  printf("connecting to %s %d\n", argv[1], server_addr.sin_port );

	if( -1 == connect(sock_fd,
	  			        (const struct sockaddr *) &server_addr,	sizeof(server_addr))){
	  				printf("Error connecting\n");
	                exit(-1);
	}

		 
    // Create socket thread 
	
	
	//creates a windows and a board with 50x20 cases
	create_board_window(board_size[0],board_size[1]);

	

	while (!done){
		while (SDL_PollEvent(&event)) {
			if(event.type == SDL_QUIT) {
					done = SDL_TRUE;
			}
			if(event.type == Event_ShowCharacter){
				
				printf("new event received\n");
			}

			
			if(event.type == SDL_MOUSEMOTION){
				//
				}
			}
		}
	
	printf("Fim\n");
	close_board_windows();
	exit(0);
}
