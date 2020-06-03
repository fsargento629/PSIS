#ifndef SERVER_H
#define SERVER_H
#include "common.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>


game_object_struct** board; // board global variable
int board_size[2]; 
int maxplayers; 
int* client_fd_list; //list of client socket fd's
int* client_score_fd; // list of client socket fd's for socre comms
int* superpacman_tokens; //tokens for every player's pacman
int player_connections; // //numbers of connected players
pthread_mutex_t board_lock; //to lock all board related functions
board_struct board_data; // stores the board pointer and the size of the board
int*scores; // score for every client


#endif
