#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdbool.h>
#include <SDL2/SDL.h>

bool graphics_init();
void graphics_shutdown();
void graphics_clear();
void graphics_present();
SDL_Texture* graphics_load_texture(const char* filename);
void graphics_draw_texture(SDL_Texture* texture, int x, int y);
void graphics_draw_sized_texture(SDL_Texture* texture, int x, int y, int width, int height);
void graphics_scale_texture(SDL_Texture* texture, int x, int y, int scale);
void graphics_draw_text(const char* text, int x, int y, char* font_path, int font_size, SDL_Texture* text_bg, int alpha, int r, int g, int b);
void graphics_play_music(const char* filename);
void graphics_fade_in(SDL_Texture* texture, int alpha);
SDL_Texture* combine_textures(SDL_Texture* textures[*], int x_positions[*], int y_positions[*], int x_widths[*], int y_heights[*], int number_of_buttons);
SDL_Texture* create_transparent_rect_texture(int width, int height, int alpha, int r, int g, int b);
SDL_Texture* graphics_load_text(char* text);
SDL_Texture* graphics_resize_texture(SDL_Texture* texture, int new_width, int new_height);

extern SDL_Renderer* gRenderer; // shared
extern SDL_Window* gWindow;
extern int WINDOW_HEIGHT;
extern int WINDOW_WIDTH;
#endif
