#ifndef SCRIPT_H
#define SCRIPT_H

#include <stdbool.h>

bool script_load(const char* filename);
void script_unload();
void script_next();
void script_render();
void script_update();
void script_choice_click(int index);

#endif

