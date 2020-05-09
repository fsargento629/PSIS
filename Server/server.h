#include "common.h"
#define BOARDTXT "board.txt"
#define DEFAULT_COLOR 1
#define TOKEN_REGEN 1
#define TOKEN_COOLDOWN 0.5
#define INACTIVITY_TIME 10 //it is 10 just for debugging 
#define NO_FRUIT -2
#define FRUIT_WAITING -1

game_state_struct game_state;
game_object_struct** board;
int board_size[2];
int client_fd_list[MAXPLAYERS];

int player_connections;
pthread_mutex_t board_lock;
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

typedef struct token_data_struct
{
  int* move_tokens;
  struct timeval* t0;
  struct timeval* tf;
}token_data_struct;

typedef struct fruit_thread_args
{
  int size_x;
  int size_y;
  game_object_struct** board;
}fruit_thread_args;



typedef struct fruit_struct
{
   int x;
   int y;
   time_t t0;

}
fruit_struct;


board_data_struct board_data;


board_data_struct read_board_data(char* file_name);//
int init_server();//
void* accept_thread(void* arg);//
void* client_thread(void* arg);//
int update_board(int player,C2S_message msg);
int send_initial_message(int client_fd,int player_num);
int init_player_position(int player_num,int do_pacman,int do_monster,int pacman_color,int monster_color);
int update_clients();//
int send_game_state(int client_fd);//
void* token_refill_thread(void*arg);
void* fruit_thread(void*arg);
int generate_fruit(int x,int y,int type,game_object_struct** board);
