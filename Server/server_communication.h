#ifndef SERVER_COMMUNICATION_H
#define SERVER_COMMUNICATION_H
#include <sys/types.h>  
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "server.h"
#include "common.h"

#define MAX_CONNECTIONS 100
#define SCORE_THREAD_COOLDOWN 10
typedef struct client_thread_args
{
   int player_num;
   int fd;
   int success;
}client_thread_args;

int init_server();
void* accept_thread(void* arg);
void* client_thread(void* arg);
int send_initial_message(int client_fd,int player_num);
int update_clients();
int send_game_state(int client_fd,game_object_struct* vector,int size);
void* accept_score_thread(void* arg);
void* send_score_thread(void* arg);
void disconnect(int player_id);
void signal_callback_handler(int signum);//
vector_struct board2vector();
#endif
