#include "common.h"
#define BOARDTXT "board.txt"
game_state_struct game_state;

int server_socket;

typedef struct board_data_struct
{
   int board_size[2];
}board_data_struct;


typedef struct client_thread_args
{
   int fd;
}client_thread_args;

board_data_struct read_board_data(char* file_name);
void init_server();
void* accept_thread(void* arg);
void* client_thread(void* arg);
