#ifndef DROPDOWN_H
#define DROPDOWN_H

#include <SDL2/SDL.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

typedef enum {
    DROPDOWN_TYPE_RESOLUTION
} DropdownType;

typedef struct DropdownOption {
    char* text;
    char* action;
} DropdownOption;

typedef struct {
    char* label;
    DropdownType type;
    char* font;
    int font_size;
    int color[4];
    int x_pos;
    int y_pos;

    DropdownOption* options;
    int option_count;

    int is_open; // whether the dropdown is open
    float animation_progress; // 0.0 (closed) to 1.0 (fully open)

    SDL_Texture* background;
} Dropdown;

Dropdown* assemble_dropdown_xml(xmlNode* dropdownNode);
void render_dropdown(Dropdown* dropdown);
void handle_dropdown_click(Dropdown* dropdown, int mouse_x, int mouse_y, void (*action_handler)(const char* action));

#endif
