#ifndef ENGINE_H
#define ENGINE_H

#include <stdbool.h>

extern bool awaiting_choice;
extern bool menu_visible;
extern bool page_show;

bool engine_init();
void engine_run();
void engine_shutdown();

extern int player_speed;

#endif

