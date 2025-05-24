#ifndef MAP_H
#define MAP_H

#include <stdbool.h>
#include <SDL2/SDL.h>

bool load_map(char* filename);
int clamp(int value, int _min, int _max);
void move_camera(int player_x, int player_y);
void render_map(SDL_Renderer* renderer, SDL_Texture* textures[3]);
void assign_textures(SDL_Texture** textures, int textures_length);
void render_map_from_file(SDL_Renderer* renderer, SDL_Texture* textures[3], char** current_map);
char** load_2D_map(char* filename);

#endif
