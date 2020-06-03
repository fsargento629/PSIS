#ifndef SERVER_H
#define SERVER_H
#include "common.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

/*typedef struct board_data_struct
{
   int board_size[2];
   game_object_struct** board;
}board_data_struct;*/



game_object_struct** board; // board global variable
int board_size[2];
int maxplayers;
int* client_fd_list;
int* client_score_fd;
int* superpacman_tokens;
int player_connections;
pthread_mutex_t board_lock;
board_struct board_data;
int*scores;


#endif
