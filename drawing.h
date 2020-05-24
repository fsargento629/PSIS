#ifndef DRAWING_H
#define DRAWING_H
#include <SDL2/SDL.h>
#include "client.h"
#include "UI_library.h"


void update_screen(vector_struct old_vector,vector_struct new_vector);
void draw_object(game_object_struct object,int x,int y);
int* char2color(char color);
int isMouseOnWindow(SDL_Window* window);
void initial_draw(board_struct board);
int find_object_in_vector(vector_struct old_vector,int type,int player,int* x,int* y);

#endif
