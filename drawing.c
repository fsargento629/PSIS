#include "drawing.h"
#include "client_communication.h"

//updates the screen and board
void update_screen(vector_struct old_vector,vector_struct new_vector){
    //draw new positions, if they are different
    int x,y,i,j;
    int found=0;
    int* updated_cells=calloc(old_vector.size,sizeof(int));
    game_object_struct* draw_vector=calloc(new_vector.size,sizeof(game_object_struct));
    game_object_struct* clear_vector=calloc(old_vector.size,sizeof(game_object_struct));
    /*Draw ALgorithm:
    1)For every new_vector cell, if position changed, add it to draw_vector, and add old position to clear_vector
        if position is the same, do nothing
        if cant find, add to the draw vector
    2)For every position in the ol_vector that was not found in new vector, add to clear_vector
    3) For every cell in the clear vector, clear the screen
    4) For every cell in the draw vector, draw    */
    int clear_vector_id=0;
    int draw_vector_id=0;
    // pass 1 - Search moves and new additions in new_vector
    for(i=0;i<new_vector.size;i++){
        //search for entry in the old vector
        found=0;
        for(j=0;j<old_vector.size;j++){
            if(old_vector.data[j].color==new_vector.data[i].color&&old_vector.data[j].type==new_vector.data[i].type){
                updated_cells[j]=1;
                found=1;
                if(old_vector.data[j].x==new_vector.data[i].x&&old_vector.data[j].y==new_vector.data[i].y)//do nothing
                    break;
                else{//add new position to draw vector and add old position to clear vector
                        clear_vector[clear_vector_id]=old_vector.data[j];
                        draw_vector[draw_vector_id]=new_vector.data[i];
                        clear_vector_id++;
                        draw_vector_id++;
                        break;
                }                

            }
        }
        if(found==0){//add to draw_vector
            draw_vector[draw_vector_id]=new_vector.data[i];
            draw_vector_id++;

        }
    }

    //pass 2 - Search deleted items in old vector
    for(j=0;j<old_vector.size;j++){
        if(updated_cells[j]==0){//add to clear_vector
            clear_vector[clear_vector_id]=old_vector.data[j];
            clear_vector_id++;
        }
    }

    //pass3 - clear places that need to be cleaned
    int clear_size=clear_vector_id;
    for(j=0;j<clear_size;j++){
        clear_place(clear_vector[j].x,clear_vector[j].y);
    }

    //pass4 - draw places that need to be drawn
    int draw_size=draw_vector_id;
    for(i=0;i<draw_size;i++){
        draw_object(draw_vector[i],draw_vector[i].x,draw_vector[i].y);  
    }


}


//draw an object
void draw_object(game_object_struct object,int x, int y){
    

    int*color=char2color(object.color);
    if(object.type==EMPTY)//empty
        clear_place(x,y);

    
    

    else if(object.type==PACMAN)//pacman
        paint_pacman(x,y,color[0],color[1],color[2]);

    else if(object.type==MONSTER)//mosnter
        paint_monster(x,y,color[0],color[1],color[2]);
    
    else if(object.type==BRICK)//brick
        paint_brick(x,y);
    
    else if(object.type==CHERRY)//cherry
        paint_cherry(x,y);    
    
    else if(object.type==LEMON)//lemon
        paint_lemon(x,y);
    else if(object.type==SUPERPACMAN)
        paint_powerpacman(x,y,color[0],color[1],color[2]);

    free(color);
    //printf("Object drawn\n");
}

// converts a char to a 3x1 RGB vector 
int* char2color(char color){
    int* output=calloc(3,sizeof(int));

    if(color=='r'){//red
        output[0]=255;
        output[1]=0;
        output[2]=0;
        return output;
    }

    else if(color=='g'){//green
        output[0]=0;
        output[1]=255;
        output[2]=0;
        return output;
    }

    else if(color=='c'){//cyan
        output[0]=0;
        output[1]=0;
        output[2]=255;
        return output;
    }

    else if(color=='b'){//black
        output[0]=0;
        output[1]=0;
        output[2]=0;
        return output;
    }

    else if(color=='y'){//yellow
        output[0]=200;
        output[1]=200;
        output[2]=0;
        return output;
    }

    else if(color=='o'){//orange
        output[0]=255;
        output[1]=140;
        output[2]=0;
        return output;
    }
    if(color=='b'){//black
        output[0]=255;
        output[1]=255;
        output[2]=255;
        return output;
    }
    else{
        output[0]=255;
        output[1]=0;
        output[2]=0;
        return output;
    }
    
}

int isMouseOnWindow(SDL_Window* window){
//Retorna 1 se o rato estiver dentro do ecra, 0 se fora

    int x,y;
    int board_x,board_y;

    SDL_GetWindowPosition(window,&board_x, &board_y);
    SDL_GetGlobalMouseState(&x, &y);
        
    if( (x >= board_x && x<= board_x+25*board_size[0]) && (y >= board_y && y<= board_y+25*board_size[1])){
       	return 1;
       }else
       	   return 0;

}

// draws the initial state of the matrix
void initial_draw(board_struct board){
    int x,y;
    for(y=0;y<board.size_y;y++){
        for(x=0;x<board.size_x;x++){
            draw_object(board.board[y][x],x,y);
        }
    }
}
// returns 0 if not found, and 1 if found. if found, saves position in x and y
int find_object_in_vector(vector_struct old_vector,int type,int player,int* x,int* y){
    int i;
    for(i=0;i<old_vector.size;i++){
        if(old_vector.data[i].type==type && old_vector.data[i].player==player){
            *x=old_vector.data[i].x;
            *y=old_vector.data[i].y;
            return 1;
        }
    }
    return 0;

}
