#define MAXPLAYERS 5
#define MAXOBJECTS 100
typedef struct game_object{
    int type;
    int color;
    int pos[2];
    
};

typedef struct game_state{
    int scores[MAXPLAYERS];
    game_object objects[MAXOBJETCS];
}game_state;

typedef struct setup_message{
    int board_size[2];
    int player_num;
    game_state board;
} setup_message;
