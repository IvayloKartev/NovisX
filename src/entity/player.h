#ifndef PLAYER_H
#define PLAYER_H

#include <stdbool.h>
#include <SDL2/SDL.h>

void move_player(char* direction, int speed);
void load_player_sprites(char* filename_up, char* filename_down, char* filename_right, char* filename_left);
void render_player();
void set_player_gravity(const char* dir);
void update_player();
void enable_player_jump(float height);
void player_jump();
void switch_resolution();

extern bool show_player;
extern bool can_jump;
extern bool has_gravity;

#endif // PLAYER_H
