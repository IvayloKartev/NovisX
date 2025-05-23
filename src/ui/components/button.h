#ifndef BUTTON_H
#define BUTTON_H

#include <SDL2/SDL.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

typedef struct {
    char* text;
    char* action;
    char* font;
    int font_size;
    int color[4];
    int x_pos;
    int y_pos;
    int width;
    int height;

    SDL_Texture* background;
} Button;

Button* assemble_button_xml(xmlNode* buttonNode);
void render_button(Button* button);
void handle_button_click(Button* button, int mouse_x, int mouse_y, void (*action_handler)(const char* action));

#endif

