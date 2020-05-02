#define SERVER_ADDRESS "192.168.5.66"

typedef struct socket_thread_args{
    game_state_struct* new_game_state_pt;
    int* sock_fd_pt;
    Uint32 Event_screen_refresh;

}socket_thread_args;



setup_message setup_comm(char* server_ip,int* sock_fd,char* port,
                                struct sockaddr_in* server_addr);
void* sock_thread(void* args);                                
void inform_server(game_object_struct game_object,int sock_fd);
void update_screen(game_state_struct* game_state,game_state_struct* new_game_state);
int object_changed(game_object_struct old,game_object_struct new);
void draw_object(game_object_struct object);
