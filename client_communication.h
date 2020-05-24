#ifndef CLIENT_COMMUNICATION_H
#define CLIENT_COMMUNICATION_H
#include "client.h"
#include "Server/common.h"
#include "UI_library.h"
typedef struct socket_thread_args{
    int sock_fd;
    Uint32 Event_screen_refresh;

}socket_thread_args;





int setup_comm(char* server_ip,char* port,board_struct* new_board,char* pacman_color,char* monster_color);
void* sock_thread(void* args);                                
int send_move(int x,int y,int type);
int receive_initial_game_state(board_struct* new_board,int socket_fd);
void print_score_board(int* score,int players);
void* receive_score_thread(void*arg);
int move_monster(SDL_Keycode keycode,vector_struct vector);
void client_signal_kill_handler(int signum);
#endif 
