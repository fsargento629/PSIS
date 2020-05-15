#ifndef CLIENT_H
#define CLIENT_H
#include "Server/common.h"
#define SERVER_ADDRESS "192.168.5.66"
#define SERVER_PORT 3000
#define CLIENT_SLEEP 1
#define SOCKTHREAD_USLEEP 1
#define SCORE_THREAD_SLEEP 100

// client global vars
int sock_fd;
int board_size[2];
int player_id;

#endif
