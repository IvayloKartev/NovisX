#ifndef MENU_H
#define MENU_H

#include <stdbool.h>
#include <SDL2/SDL.h>

void init_menu(const char* filename);
bool render_menu();
SDL_Texture* build_menu();
bool menu_event_handler(SDL_Event* e);

#endif // MENU_H
