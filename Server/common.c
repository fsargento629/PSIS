#include <stdio.h>
#include <stdlib.h>
#include <sys/un.h>
#include <unistd.h>
#include "common.h"
#include <pthread.h>

// handles some signals, like SIGPIPE
/*void signal_kill_handler( int signum){
    printf("Shutting down due to Ctrl-C signal\n");
    exit(0);
}*/
// used to process a disconnect
void signal_callback_handler(int signum){
}

//returns if a board space is empty or not
int is_empty(int x,int y,game_object_struct** board){
    if(board[y][x].type==0)
        return 1;
    return 0;
}
// returns a malloced 2D vector conatining the position of the desired object on the board
// returns -1 if unsuccessfull
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
    if(found==1){
        pos[0]=x;
        pos[1]=y;
    }
    else{
        pos[0]=-1;
        pos[1]=-1;
    }
    return pos;
}

// returns 1 if objs are different and zero if they are the same
int objects_are_different(game_object_struct obj1,game_object_struct obj2){
    if(obj1.type!=obj2.type ||obj1.color!=obj2.color ||obj1.player!=obj2.player||obj1.pos[0]!=obj2.pos[0]||obj1.pos[1]!=obj2.pos[1])
        return 1;
    return 0;
}

//copies new_boad to old_board
int board_copy(game_object_struct** old_board,game_object_struct** new_board,int size_x,int size_y){
    int x,y;
    if(new_board==NULL)
        return -1;
    if(old_board==NULL){
        printf("trololol\n");
        old_board=malloc(sizeof(game_object_struct*)*size_y);
        for(y=0;y<size_y;y++)
            old_board[y]=malloc(sizeof(game_object_struct)*size_x);
    }
        
    for(y=0;y<size_y;y++){
        for(x=0;x<size_x;x++){
            old_board[y][x]=new_board[y][x];
        }
    }

    return 0;
}

//frees all the lines of board
int free_board(game_object_struct** board,int size_x,int size_y){
    game_object_struct* ptr;
    int i;
    for(i=0;i<size_y;i++){
        ptr=board[i];
        free(ptr);
    }
    return 0;
}
