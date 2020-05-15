#include "board_handling.h"
#include "server.h"
#include "server_communication.h"
// initializes a pacman or a monster for a given player
int init_player_position(int player_num,int do_pacman,int do_monster,int pacman_color,int monster_color){
    //give the player a pacman and a monster in random, not filled position:
    int x=0,y=0;
    int *pos;
    if(do_pacman==1){
        do
        {
            x=rand()%board_size[0];
            y=rand()%board_size[1];
        } while(!is_empty(x,y,board));
        //if there is a pacman of the same player anywhere, delet it
        pos=find_object(player_num,PACMAN,board,board_size[0],board_size[1]);
        if(pos[0]!=-1&&pos[1]!=-1)//if it is found, delete it
            clear_board_cell(pos[0],pos[1],board);
        free(pos);
        //create pacman
        board[y][x].player=player_num;
        board[y][x].type=1;
        board[y][x].pos[0]=x;
        board[y][x].pos[1]=y;
        board[y][x].color=pacman_color;
        
        
    }
    
    if(do_monster==1){
        do
        {
            x=rand()%board_size[0];
            y=rand()%board_size[1];
        } while(!is_empty(x,y,board));
        //if there is a monster of the same player anywhere, delete it
        pos=find_object(player_num,MONSTER,board,board_size[0],board_size[1]);
        if(pos[0]!=-1&&pos[1]!=-1)//if it is found, delete it
            clear_board_cell(pos[0],pos[1],board);
        free(pos);
        //create monster
        board[y][x].player=player_num;
        board[y][x].type=2;
        board[y][x].pos[0]=x;
        board[y][x].pos[1]=y;
        board[y][x].color=monster_color;
    }
        
   
    return 0;//if found  space for both the pacman and moster

}

//reads board file and creates a board matrix
void read_board_data(char*file_name){
    int i_x,i_y;
    int x=0,y=0;
    char buff[500],ch;
    FILE* fp=fopen("board.txt","r");
    fgets(buff,sizeof(buff),fp);
    sscanf(buff,"%d %d",&x,&y);
    board_data.board_size[0]=x;
    board_data.board_size[1]=y;
    board_size[0]=x;
    board_size[1]=y;
    board=malloc(sizeof(game_object_struct*)*y);
    // read bricks:
    i_x=0;
    i_y=0;
    board[i_y]=malloc(sizeof(game_object_struct)*x);
    while ((ch = fgetc(fp)) != EOF)
    {
        if(ch=='\n'){
            //printf("  (%d,%d)\n",i_x,i_y);
            i_x=0;;
            i_y++;;
            board[i_y]=malloc(sizeof(game_object_struct)*x);
                        
        }
        else if(ch=='B'){
            board[i_y][i_x].color=0;
            board[i_y][i_x].player=-1;
            board[i_y][i_x].pos[0]=i_x;
            board[i_y][i_x].pos[1]=i_y;
            board[i_y][i_x].type=3;//brick
            //printf("B");
            i_x++;
        }
        else{
            board[i_y][i_x].color=0;
            board[i_y][i_x].player=-1;
            board[i_y][i_x].pos[0]=i_x;
            board[i_y][i_x].pos[1]=i_y;
            board[i_y][i_x].type=0;//empty
            //printf("_");
            i_x++;
        }

        board_data.board=board;
        
           

    }
    

}
/* 
    Movement Rules:
    0)If the following block is empty, just move to it, i.e., switch places with it
    1) If there is a brick in the way, bounce back
    2) If two objects of the same player collide, they switch places
    3) If two pacmen, or two monsters, collide, they change places
    4) If a superpacman collides into a monster, it is eaten and put in a different position. The score is incremented
    5) If a monster collides into a pacman, the pacman gets eaten and is put in a different position. The score is incremented
    6) If a pacman and a fruit collide, it eats the fruit and becomes a superpacman
    7) If a monster and a fruit collide, the fruit just disappears
    
*/
// Updates the board after receiving a client message
int update_board(int player_num,C2S_message msg){
    int* pos,*next_pos;
    char aux_color;
    int ret=0;//amount of movement tokens used
    //find matching pacman or moster and update its position
    pthread_mutex_lock(&board_lock);
    pos=find_object(player_num,msg.type,board,board_size[0],board_size[1]);

    if(pos[0]==OBJECT_NOT_FOUND|| pos[1]==OBJECT_NOT_FOUND){//if object is not found, exit function
        pthread_mutex_unlock(&board_lock);
        free(pos);
        return 0 ;
    }
    next_pos=calloc(2,sizeof(int));
    next_pos[0]=msg.x;
    next_pos[1]=msg.y;
    //Convert pacman/monster desired position to a position next to the pacman
    closest_square(pos[0],pos[1],next_pos,board_size[0],board_size[1],board);   


    //Rule 1-Bounce. After applying rule1 the other rules still apply
    if(board[next_pos[1]][next_pos[0]].type==BRICK){
        ret=bounce_back(pos,next_pos,board,board_size[0],board_size[1]);
    }

    //Rule 0- If spot is empty, move to it
    if(is_empty(next_pos[0],next_pos[1],board)){
        switch_places(pos,next_pos,board);
        pthread_mutex_unlock(&board_lock);
        free(pos);
        free(next_pos);
        return 1;
    }

    //Save player and type, to make the code easier to read =)
    int player1=board[pos[1]][pos[0]].player;
    int player2=board[next_pos[1]][next_pos[0]].player;
    int type1=board[pos[1]][pos[0]].type;//object that wants to move
    int type2=board[next_pos[1]][next_pos[0]].type;//object that is in the way, potentially

    //Rule 2-objects of the same player change positions  
    if(player1==player2&&player1>=0){
        switch_places(pos,next_pos,board);
        ret=1;
    }

    //Rule 3-two pacmen or two monster collide-> they switch places
    
    else if(  ((type1==type2)&&(type1==PACMAN||type1==MONSTER||type1==SUPERPACMAN)) || (type1==PACMAN&&type2==SUPERPACMAN)||(type1==SUPERPACMAN&&type2==PACMAN)  ){//both are pacmen, superpcamen or monsters
        switch_places(pos,next_pos,board);
        ret=1;
        
    }
  
    //Rule 4 and 5- Monster eats pacman and superpacman eats monster
    else if(type1==MONSTER&&type2==PACMAN){
        aux_color=board[next_pos[1]][next_pos[0]].color;
        eat(pos,next_pos,board);
        scores[player1]++;
        init_player_position(player2,1,0,aux_color,0);
        ret=1;
    }
    else if(type1==PACMAN&&type2==MONSTER){
        aux_color=board[pos[1]][pos[0]].color;
        eat(next_pos,pos,board);
        scores[player2]++;
        init_player_position(player1,1,0,aux_color,0);//do only pacman
        ret=1;
    }
    else if(type1==SUPERPACMAN&&type2==MONSTER){
        aux_color=board[next_pos[1]][next_pos[0]].color;
        eat(pos,next_pos,board);
        scores[player1]++;
        init_player_position(player2,0,1,0,aux_color);//do monster
        superpacman_tokens[player1]--;
        if(superpacman_tokens[player1]<=0)
            board[next_pos[1]][next_pos[0]].type=PACMAN;
        ret=1;
    }
    else if(type1==MONSTER&&type2==SUPERPACMAN){
        aux_color=board[pos[1]][pos[0]].color;
        eat(next_pos,pos,board);
        scores[player2]++;
        init_player_position(player1,0,1,0,aux_color);//do only monster
        superpacman_tokens[player2]--;
        if(superpacman_tokens[player2]<=0)
            board[next_pos[1]][next_pos[0]].type=PACMAN;
        ret=1;
    }

    // Rule 6 and 7 - Eat cherry
    else if(type1==PACMAN&&(type2==CHERRY||type2==LEMON)){
        eat(pos,next_pos,board);
        board[next_pos[1]][next_pos[0]].type=SUPERPACMAN; 
        superpacman_tokens[player1]=SUPERPACMAN_IMMUNITY;
        ret=1;
    }
    else if((type2==CHERRY||type2==LEMON)&&(type1==MONSTER||type1==SUPERPACMAN)){
        eat(pos,next_pos,board);
        ret=1;
    }

// End of movement rules
    pthread_mutex_unlock(&board_lock);
    free(pos);
    free(next_pos);
    return ret;
}

// generates a fruit ina given board position. Returns 0 if impossible
int generate_fruit(int x,int y,int type,game_object_struct** board){

    pthread_mutex_lock(&board_lock);
    if(board[y][x].type!=EMPTY){
        pthread_mutex_unlock(&board_lock);
        return 0;
    }
    else{
        board[y][x].player=-1;
        board[y][x].pos[0]=x;   
        board[y][x].pos[1]=y;
        board[y][x].type=type;
        pthread_mutex_unlock(&board_lock);
        return 1;
    }
}


// handles all fruit (re)generation
void* fruit_thread(void*arg){
    fruit_thread_args args=*(fruit_thread_args*)arg;
    int size_x=args.size_x;
    int size_y=args.size_y;
    int x,y,type;
    game_object_struct** board=args.board;
    fruit_struct* fruit_vector=calloc(2*(maxplayers-1),sizeof(fruit_struct));
    int i,j;
    //initialize every position at -2, to signal empty;
    for(i=0;i<2*(maxplayers-1);i++){
        fruit_vector[i].x=NO_FRUIT;
        fruit_vector[i].y=NO_FRUIT;
    }
    int active_fruits=0;
    
    time_t tf;
    while(1){
        usleep(FRUIT_THREAD_SLEEP*1000);
        if(active_fruits<(2*(player_connections-1))){
            type=rand()%2+CHERRY;//generates rand num between CHERRY and LEMON
            do
            {
                x=rand()%size_x;
                y=rand()%size_y;
                
            } while (generate_fruit(x,y,type,board)==0);
            i=0;
            while(fruit_vector[i].x!=NO_FRUIT)//find 1st empty vector position
                i++;
            fruit_vector[i].x=x;
            fruit_vector[i].y=y;
            //printf("%d;generated a fruit\n",active_fruits);
            active_fruits++;

        }

          //see if any fruit is missing
        time(&tf);//save crurrent time in tf
        for(i=0;i<2*(maxplayers-1);i++){

            if(fruit_vector[i].x==NO_FRUIT)
                continue;//-2 implies there is no fruit

            else if(fruit_vector[i].x==FRUIT_WAITING){
                //check timer
                if(difftime(tf,fruit_vector[i].t0)>=2){
                    //generate fruit and save it in the fruit vector
                    type=rand()%2+CHERRY;
                     do
                        {
                            x=rand()%size_x;
                            y=rand()%size_y;
                            
                        } while (generate_fruit(x,y,type,board)==0);
                        
                        j=0;
                        while(fruit_vector[j].x!=NO_FRUIT)//find 1st empty vector position
                            j++;
                        fruit_vector[i].x=x;
                        fruit_vector[i].y=y;
                        //active_fruits++;

                }

            }
            


            else if(board[fruit_vector[i].y][fruit_vector[i].x].type!=CHERRY &&
            board[fruit_vector[i].y][fruit_vector[i].x].type!=LEMON){

                if(active_fruits>(2*(player_connections-1))){//there are too many fruits
                    fruit_vector[i].x=NO_FRUIT;
                    fruit_vector[i].y=NO_FRUIT;
                    active_fruits--;
                    

                }
                //start timer for that fruit, if there are not too many fruits already
                else {
                    time(&fruit_vector[i].t0);
                    fruit_vector[i].x=FRUIT_WAITING;
                    fruit_vector[i].y=FRUIT_WAITING;
                }
            }
        }

      
    }

}


//empties a board cell
void clear_board_cell(int x,int y,game_object_struct** board){
    board[y][x].color=0;
    board[y][x].player=0;
    board[y][x].type=0;
}

//converts a clients' move request to a reasonable move request
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



}


//bounces an object back and returns if the object moved or not
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

//switch two objects in the board
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

//returns time_diff in seconds
double time_delta(struct timeval* tf,struct timeval* t0  ){
    double delta;
    delta=(tf->tv_sec - t0->tv_sec)+((tf->tv_usec - t0->tv_usec)/1000.0/1000.0);
    return delta;
}
