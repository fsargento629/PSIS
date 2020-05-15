#ifndef DRAWING_H
#define DRAWING_H
#include <SDL2/SDL.h>
#include "client.h"
#include "UI_library.h"
void update_screen(game_object_struct** old_board,game_object_struct** new_board,int override);
void draw_object(game_object_struct object,int x,int y);
int* char2color(char color);

#endif
