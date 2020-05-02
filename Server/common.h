#define MAXPLAYERS 5
#define MAXOBJECTS 100


typedef struct game_object_struct{
    int type;// 0-> empty ;1->pacman;2->mosnter;3->brick;4->cherry;5->lemon
    int color;
    int pos[2];
    
}game_object_struct;

typedef struct game_state_struct{
    int scores[MAXPLAYERS];
    game_object_struct objects[MAXOBJECTS];
}game_state_struct;

typedef struct setup_message{
    int board_size[2];
    int player_num;
    int pacman_num;
    int monster_num;
    game_state_struct game_state;
} setup_message;

typedef struct C2S_message{
    int type;
    int x;
    int y;
}C2S_message;

typedef struct S2C_message{
    game_state_struct game_state;
}S2C_message;


