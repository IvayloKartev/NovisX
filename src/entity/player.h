#ifndef PLAYER_H
#define PLAYER_H

#include <stdbool.h>
#include <SDL2/SDL.h>

void move_player(char* direction, int speed);
void load_player_sprites(char* filename_up, char* filename_down, char* filename_right, char* filename_left);
void render_player();

extern bool show_player;
#endif // PLAYER_H
