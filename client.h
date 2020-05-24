#ifndef CLIENT_H
#define CLIENT_H
#include "Server/common.h"
#define SERVER_ADDRESS "192.168.5.66"
#define SERVER_PORT 3000
#define CLIENT_SLEEP 1
#define SOCKTHREAD_USLEEP 1
#define SCORE_THREAD_SLEEP 1000




// client global vars
int sock_fd;
int board_size[2];
int player_id;
board_struct raw_board;
game_object_struct** board;

#endif
