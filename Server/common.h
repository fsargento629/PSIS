#ifndef COMMON_H
#define COMMON_H

// Defines used by both the client and server 
#define DEFAULT_SERVER_PORT 3000
#define DEFAULT_SCORE_SERVER_PORT 3001
#define EMPTY 0
#define PACMAN 1
#define MONSTER 2
#define BRICK 3
#define CHERRY 4
#define LEMON  5
#define SUPERPACMAN 6
#define OBJECT_NOT_FOUND -1

//structs used by both the server and client

//struct used for the board matrix
typedef struct game_object_struct{
    int type;// 0-> empty ;1->pacman;2->monster;3->brick;4->cherry;5->lemon;6->superpac
    char color;
    int x;
    int y;
    int player;
    
}game_object_struct;

//struct used to send the first message to the client
typedef struct setup_message{//to send to client on startup
    int board_size[2];
    int player_num;
    int max_players;
} setup_message;

//struct used for the client-to-server messages
typedef struct C2S_message{//Client to Server message 
    int type;
    int x;
    int y;
}C2S_message;   

typedef struct board_struct
{
    game_object_struct** board;
    int size_x;
    int size_y;
}board_struct;

typedef struct vector_struct
{
   int size;
   game_object_struct* data;
}vector_struct;


// Functions called by both the server and client
//void signal_kill_handler( int signum);//
#endif
