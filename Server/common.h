#ifndef COMMON_H
#define COMMON_H

// Defines used by both the client and server
#define MAXPLAYERS 5 //remove
#define MAXOBJECTS 100 //remove
#define DEFAULT_SERVER_IP "192.168.5.66"
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
    int type;// 0-> empty ;1->pacman;2->mosnter;3->brick;4->cherry;5->lemon;6->superpac
    char color;
    int pos[2];
    int player;
    
}game_object_struct;

//struct used to send the first message to the client
typedef struct setup_message{//to send to client on startup
    int board_size[2];
    int player_num;
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



// Functions called by both the server and client
void signal_callback_handler(int signum);//
void signal_kill_handler( int signum);//
int is_empty(int x,int y,game_object_struct** board);
int* find_object(int player,int type,game_object_struct** board,int size_x,int size_y);
int objects_are_different(game_object_struct obj1,game_object_struct obj2);
int board_copy(game_object_struct** old_board,game_object_struct** new_board,int size_x,int size_y);
int free_board(game_object_struct** board,int size_x,int size_y);
#endif
