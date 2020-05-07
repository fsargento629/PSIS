#include "common.h"
#define BOARDTXT "board.txt"
<<<<<<< HEAD
//#define DEFAULT_COLOR 1
=======
#define DEFAULT_COLOR 1

>>>>>>> 0a19062ee810e2040d9251b9f028039be9b24806
game_state_struct game_state;
game_object_struct** board;
int board_size[2];
int client_fd_list[MAXPLAYERS];

int player_connections;

typedef struct board_data_struct
{
   int board_size[2];
   game_object_struct** board;
}board_data_struct;


typedef struct client_thread_args
{
   int player_num;
   int fd;
   int success;
}client_thread_args;

board_data_struct board_data;

board_data_struct read_board_data(char* file_name);//
int init_server();//
void* accept_thread(void* arg);//
void* client_thread(void* arg);//
void update_board(int player,C2S_message msg);
int send_initial_message(int client_fd,int player_num);//
int init_player_position(int player_num);//
int update_clients();//
int send_game_state(int client_fd);//

