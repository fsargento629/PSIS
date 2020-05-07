#include <sys/types.h>  
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include"server.h"
#include <sys/un.h>
#include <unistd.h>


void signal_kill_handler( int signum){
    printf("Shutting down due to Ctrl-C signal\n");
    exit(0);
}

void signal_callback_handler(int signum){
}



int is_empty(int x,int y,game_object_struct** board){
    if(board[y][x].type==0)
        return 1;
    return 0;
}
//this function empties a board cell
void clear_board_cell(int x,int y,game_object_struct** board){
    board[y][x].color=0;
    board[y][x].player=0;
    board[y][x].type=0;
}

//This function returns a 1x2 vector containing the position of the desired object 
int* find_object(int player,int type,game_object_struct** board,int size_x,int size_y){
    int x,y;
    int found=0;
    for(y=0;y<size_y;y++){
        for(x=0;x<size_x;x++){
            if(board[y][x].player==player && board[y][x].type==type){
                 found=1;
                 break;
             }
                     
        }
        if(found==1)
            break;  

    }

    int* pos=malloc(2*sizeof(int));
    pos[0]=x;
    pos[1]=y;
    return pos;
}

// returns 1 if objs are different and zero if they are the same
int objects_are_different(game_object_struct obj1,game_object_struct obj2){
    if(obj1.type!=obj2.type ||obj1.color!=obj2.color ||obj1.player!=obj2.player||obj1.pos[0]!=obj2.pos[0]||obj1.pos[1]!=obj2.pos[1])
        return 1;
    return 0;
}

void printf_game_state(int size_x,int size_y,game_state_struct* game){

    int x,y;
    if(game==NULL){
        printf("Game state is NULL\n");
        exit(-1);
    }
    if(game->board==NULL){
        printf("Board is NULL\n");
        exit(-1);
    }
    printf("----- Printing game state ----\n");
    for(y=0;y<size_y;y++){
        for(x=0;x<size_x;x++){
            if(game->board[y][x].type==0)
                printf(" ");
            else if(game->board[y][x].type==1)
                printf("P");
            else if(game->board[y][x].type==2)
                printf("M");
            else if(game->board[y][x].type==3)
                printf("B");
            else if(game->board[y][x].type==4)
                printf("C");
            else if(game->board[y][x].type==5)
                printf("L");

        }
        printf("\n");
    }

    printf("----- Game state printed ----\n");

}
