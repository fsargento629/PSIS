#define SERVER_ADDRESS "192.168.5.66"
#define SERVER_PORT 3000


//socket thread receive game_state from server.
typedef struct socket_thread_args{
    int sock_fd;
    Uint32 Event_screen_refresh;

}socket_thread_args;
//
int sock_fd;
int score[MAXPLAYERS];
int board_size[2];
//game_state_struct game_state;
//game_object_struct** board;
//game_object_struct** board_new;
int player_id;
//
int setup_comm(char* server_ip,char* port,game_state_struct* game_state);
void* sock_thread(void* args);                                
//void inform_server(game_object_struct game_object,int sock_fd);
void update_screen(game_object_struct** old_board,game_object_struct** new_board,int override);
void draw_object(game_object_struct object,int x,int y);
int send_move(int x,int y,int type);
int receive_game_state(game_state_struct* temp_game_state,int socket_fd);//
