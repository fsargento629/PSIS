#ifndef BOARD_HANDLING_H
#define BOARD_HANDLING_H
// Defines used only for board creationg and handling
#define BOARDTXT "board.txt"
#define TOKEN_COOLDOWN 0.5
#define INACTIVITY_TIME 60 
#define NO_FRUIT -2
#define FRUIT_WAITING -1
#define ACCEPT_THREAD_SLEEP 500 //in milliseconds
#define FRUIT_THREAD_SLEEP 200
#define SUPERPACMAN_IMMUNITY 2
#define SCORE_THREAD_COOLDOWN 10
#include "server.h"
#include "common.h"
#include "server_communication.h"


// structs used only for board handling
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


void read_board_data(char* file_name);//
int update_board(int player,C2S_message msg);
int init_player_position(int player_num,int do_pacman,int do_monster,int pacman_color,int monster_color);
void* fruit_thread(void*arg);
int generate_fruit(int x,int y,int type,game_object_struct** board);
void closest_square(int x,int y,int* next_pos,int size_x,int size_y,game_object_struct** board);
int bounce_back(int* pos1,int* pos2,game_object_struct** board,int size_x,int size_y);
void switch_places(int* pos,int* next_pos,game_object_struct** board);
void eat(int* predator,int*prey,game_object_struct** board);
double time_delta(struct timeval* tf,struct timeval* t0  );
void clear_board_cell(int x,int y,game_object_struct** board);

#endif