#define MAXPLAYERS 5
#define MAXOBJECTS 100
#define DEFAULT_SERVER_IP "192.168.5.66"
#define DEFAULT_SERVER_PORT 3000
#define EMPTY 0
#define PACMAN 1
#define MONSTER 2
#define BRICK 3
#define CHERRY 4
#define LEMON  5
#define SUPERPACMAN 6
#define OBJECT_NOT_FOUND -1
typedef struct game_object_struct{
    int type;// 0-> empty ;1->pacman;2->mosnter;3->brick;4->cherry;5->lemon;6->superpac
    char color;
    int pos[2];
    int player;
    
}game_object_struct;

typedef struct game_state_struct{ //is it useful?
    int scores[MAXPLAYERS];
    game_object_struct** board; 
}game_state_struct;

typedef struct setup_message{//to send to client on startup
    int board_size[2];
    int player_num;
} setup_message;

typedef struct C2S_message{//Client to Server message 
    int type;
    int x;
    int y;
}C2S_message;
//not important should it be removed??
/*typedef struct S2C_message{//Server to client message
    game_state_struct game_state;
}S2C_message;*/

void signal_callback_handler(int signum);//
void signal_kill_handler( int signum);//
int is_empty(int x,int y,game_object_struct** board);
void clear_board_cell(int x,int y,game_object_struct** board);
int* find_object(int player,int type,game_object_struct** board,int size_x,int size_y);
int objects_are_different(game_object_struct obj1,game_object_struct obj2);
void printf_game_state(int size_x,int size_y,game_state_struct* game);
void closest_square(int x,int y,int* next_pos,int size_x,int size_y,game_object_struct** board);
int bounce_back(int* pos1,int* pos2,game_object_struct** board,int size_x,int size_y);
void switch_places(int* pos,int* next_pos,game_object_struct** board);
void eat(int* predator,int*prey,game_object_struct** board);

double time_delta(struct timeval* tf,struct timeval* t0  );
