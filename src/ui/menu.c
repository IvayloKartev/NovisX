#include "../core/graphics.h"
#include "menu.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <SDL.h>
#include "../core/engine.h"
#include "../screen/page.h"

#define LEN(arr) ((int) (sizeof (arr) / sizeof (arr)[0]))

static char button_labels[10][64];
static char button_paths[10][256];
static int length = 0;
char background_path[256];
char button_bg_path[256];
SDL_Texture* bg = NULL;
SDL_Texture* btn = NULL;
SDL_Rect button_rects[10];

void init_menu(const char* filename)
{
    printf("Initializing menu");
    FILE* f = fopen(filename, "r");
    char line[512];
    // going through every command in the menu file
    while (fgets(line, sizeof(line), f))
    {
        // checks if first n symbols form a known command and adjusts variables accordingly
        if (strncmp(line, "background", 10) == 0)
        {
            sscanf(line, "background %s", background_path);
            bg = graphics_load_texture(background_path);
        }
        else if (strncmp(line, "button", 6) == 0)
        {
            int index = 0;
            char temp_str[256];
            sscanf(line, "button %d %s", &index, temp_str);
            sscanf(line, "button %d %s > %s", &index, button_labels[index-1], button_paths[index-1]);
            printf("Got %d button %s, pointing to %s\n", index, button_labels[index-1], button_paths[index-1]);
            length = index;
        }
        else if(strncmp(line, "btn_bg", 6) == 0)
        {
            sscanf(line, "btn_bg %s", background_path);
            btn = graphics_load_texture(button_bg_path);
        }
    }
    if(bg == NULL) bg = create_transparent_rect_texture(600, 100, 100, 50, 50, 50);
    if(btn == NULL) btn = create_transparent_rect_texture(100, 60, 100, 50, 50, 50);
}


SDL_Texture* build_menu()
{
    int x_pos[length+1];
    int y_pos[length+1];
    int widths[length+1];
    int heights[length+1];
    for (int i = 0; i < length + 1; i++) x_pos[i] = 0;
    for (int i = 0; i < length + 1; i++) y_pos[i] = 0;
    for (int i = 0; i < length + 1; i++) widths[i] = 175;
    for (int i = 0; i < length + 1; i++) heights[i] = 100;

    heights[0] = WINDOW_HEIGHT;
    widths[0] = 175;


    SDL_Texture* textures[LEN(button_labels)+1];

    textures[0] = bg;

    for(int i=1; i<length+1; i++)
    {
        y_pos[i] = y_pos[i-1] + 60;
        SDL_Texture* btn_text = graphics_load_text(button_labels[i-1]);

        //printf("Loaded following text: %s\n", button_labels[i-1]);

        SDL_Texture* temp_textures[2];
        temp_textures[0] = btn;
        temp_textures[1] = btn_text;

        int temp_x_pos[2];
        temp_x_pos[0] = x_pos[i];
        temp_x_pos[1] = x_pos[i] + 30;

        int temp_y_pos[2];
        temp_y_pos[0] = y_pos[i];
        temp_y_pos[1] = y_pos[i];

        int temp_widths[2];
        temp_widths[0] = widths[i];
        temp_widths[1] = widths[i] - 60;

        int temp_heights[2];
        temp_heights[0] = heights[i];
        temp_heights[1] = heights[i] - 10;

        button_rects[i-1] = (SDL_Rect){ .x = temp_x_pos[0] + 625, .y = temp_y_pos[0], .w = temp_widths[0], .h = temp_heights[0] };

        //nt length = sizeof(button_labels);
        //printf("Added button: %s\n", button_labels[i-1]);
        textures[i] = combine_textures(temp_textures, temp_x_pos, temp_y_pos, temp_widths, temp_heights, 2);
    }

    SDL_Texture* res = NULL;

    res = combine_textures(textures, x_pos, y_pos, widths, heights, length+1);
    //if(res != NULL) printf("Combined!!!");
    return combine_textures(textures, x_pos, y_pos, widths, heights, length+1);;

}

bool render_menu()
{
    SDL_Texture* menu_build = build_menu();
    if(menu_build == NULL) printf("PROBLM EHERE");
    graphics_draw_texture(menu_build, WINDOW_WIDTH - 175, 0);
    return true;
}

bool menu_event_handler(SDL_Event* e) {
    if (e->type == SDL_MOUSEBUTTONDOWN) {
        int x = e->button.x;
        int y = e->button.y;

        for (int i = 0; i < LEN(button_labels); i++) {
            if (SDL_PointInRect(&(SDL_Point){x, y}, &button_rects[i])) {
                printf("Button %d clicked: %s\n", i, button_paths[i]);
                page_show = true;
                load_page(button_paths[i]);
                //execute_button_action(button_paths[i]);
                return true; // handled
            }
        }
    }
    return false; // not handled
}


