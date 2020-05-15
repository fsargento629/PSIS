#include "drawing.h"
#include "client_communication.h"

//updates the screen 
void update_screen(game_object_struct** old_board,game_object_struct** new_board,int override){
    int x,y;
    //check for differences.
    //If there is a difference,paint it
    
    for(y=0;y<board_size[1];y++){
        for(x=0;x<board_size[0];x++){
            
            if(override){
                draw_object(new_board[y][x],x,y);
                
            }
        
            else if(objects_are_different(old_board[y][x],new_board[y][x]))
                draw_object(new_board[y][x],x,y);
        
        }
       
    }

    //printf("Screen updated\n");
    
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

