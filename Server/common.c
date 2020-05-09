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

// for a given begin point and end point, returns the first square on its path
void closest_square(int x,int y,int* next_pos,int size_x,int size_y,game_object_struct** board){
    int delta_x=next_pos[0]-x;
    int delta_y=next_pos[1]-y;

    if(abs(delta_x)>abs(delta_y)){
        if(delta_x<0)
            next_pos[0]=x-1;
        else
            next_pos[0]=x+1;
        next_pos[1]=y;
    }
    else if(abs(delta_x)<=abs(delta_y)){
        next_pos[0]=x;
        if(delta_y<0)
            next_pos[1]=y-1;
        else
            next_pos[1]=y+1;
    }

    //make sure it fits in the window

     if(next_pos[0]<0){
        if(is_empty(1,next_pos[1],board))
            next_pos[0]=1;
        else
            next_pos[0]=0;
    }
    else if(next_pos[0]>=size_x){
        if(is_empty(size_x-2,next_pos[1],board))
            next_pos[0]=size_x-2;
        else
            next_pos[0]=size_x-1;
    }
    else if(next_pos[1]<0){
        if(is_empty(next_pos[0],1,board))
            next_pos[1]=1;
        else
            next_pos[1]=0;
    }
    else if(next_pos[1]>=size_y){
        if(is_empty(next_pos[0],size_y-2,board))
            next_pos[1]=size_y-2;
        else
            next_pos[1]=size_y-1;
    }

    /*if(next_pos[0]<0)
        next_pos[0]=0;
    else if(next_pos[0]>size_x)
        next_pos=size_x;
    else if(next_pos[1]<0)
        next_pos[1]=0;
    else if(next_pos[1]>size_y)
        next_pos[1]=size_y;*/


}
// bounce back game_object in pos1 if possible, otherwise dont change position (1 is the mover, 2 is the movement position)
//returns the movement tokens used
int bounce_back(int* pos1,int* pos2,game_object_struct** board,int size_x,int size_y){
    int delta_pos[2],bounce_x,bounce_y;
    
    // delta pos vector
    delta_pos[0]=pos2[0]-pos1[0];
    delta_pos[1]=pos2[1]-pos1[1];
    //create new point
    bounce_x=pos1[0]-delta_pos[0];
    bounce_y=pos1[1]-delta_pos[1];
    //If bounce point is a possibility, bounce to it, otherwise, dont do it
    if(bounce_x>=0&&bounce_x<size_x&&bounce_y>=0&&bounce_y<size_y){
        pos2[0]=bounce_x;
        pos2[1]=bounce_y;      
        return 1; //used 1 movement token
    }
    else{ //stay put
        pos2[0]=pos1[0];
        pos2[1]=pos1[1];   
        return 0; //used zero movement tokens  
    }



}

//switch two objects in the board;
void switch_places(int* pos,int* next_pos,game_object_struct** board){
    int color, type;
    int player;
    //copy contents of game object in pos
    color=board[pos[1]][pos[0]].color;
    type=board[pos[1]][pos[0]].type;
    player=board[pos[1]][pos[0]].player;
    //change contents in pos to those of next_pos
    board[pos[1]][pos[0]].color=board[next_pos[1]][next_pos[0]].color;
    board[pos[1]][pos[0]].player=board[next_pos[1]][next_pos[0]].player;
    board[pos[1]][pos[0]].type=board[next_pos[1]][next_pos[0]].type;
    //copy the contents of pos to next pos
    board[next_pos[1]][next_pos[0]].color=color;
    board[next_pos[1]][next_pos[0]].player=player;
    board[next_pos[1]][next_pos[0]].type=type;

}

//predator eats prey, prey gets deleted and predator moves to the space of prey
void eat(int* predator,int*prey,game_object_struct** board){
    // move the predator to the prey's spot
    board[prey[1]][prey[0]].color=board[predator[1]][predator[0]].color;
    board[prey[1]][prey[0]].player=board[predator[1]][predator[0]].player;
    board[prey[1]][prey[0]].type=board[predator[1]][predator[0]].type;
    //clear the predator spot
    clear_board_cell(predator[0],predator[1],board);

}
