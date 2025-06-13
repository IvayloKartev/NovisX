#include "page.h"
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../core/parser.h"
#include "../ui/components/components.h"
#include "../core/graphics.h"
#include "../core/engine.h"
#include "../entity/player.h"

PageData page_data = {
    .background = NULL,
    .labels_count = 0,
    .dropdowns_count = 0,
    .buttons_count = 0
};

void load_page(char* filename)
{
    xmlDoc* doc = xmlReadFile(filename, NULL, 0);
    if (!doc) {
        printf("Could not parse XML file: %s\n", filename);
        return;
    }

    // XML file should have ONLY ONE root element, for example <xml></xml> - everything else should reside within it
    xmlNode* root = xmlDocGetRootElement(doc);
    parse_node(root);
    xmlFreeDoc(doc);
    xmlCleanupParser();
}

void render_page()
{
    printf("RENDERING PAGE AGIAN\n");
    if(page_data.background)
    {
        graphics_draw_sized_texture(page_data.background, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    }
    for(int i=0; i<page_data.labels_count-1; i++)
    {
        //printf("%d\n", i);
        //printf("COLOR: %d:%d:%d:%d", page_data.labels[i].color[0], page_data.labels[i].color[1], page_data.labels[i].color[2], page_data.labels[i].color[3]);
        //printf("TEXT: %s\nFONT: %s\nSIZE:%d\nX:%d\nY:%d\n", page_data.labels[i].text, page_data.labels[i].font, page_data.labels[i].font_size, page_data.labels[i].x_pos, page_data.labels[i].y_pos);
        render_label(page_data.labels[i].text, page_data.labels[i].font, page_data.labels[i].font_size, NULL, page_data.labels[i].color, page_data.labels[i].x_pos, page_data.labels[i].y_pos);
    }
    for(int i=0; i<page_data.labels_count; i++)
    {
        render_dropdown(&page_data.dropdowns[i]);
    }
    for(int i=0; i<page_data.buttons_count; i++)
    {
        render_button(&page_data.buttons[i]);
    }
}

void handler_function(const char* action)
{
    printf("Action received: %s\n", action);

    SDL_RenderClear(gRenderer);

    if (strcmp(action, "800x600") == 0)
    {
        SDL_SetWindowSize(gWindow, 800, 600);
        WINDOW_WIDTH = 800;
        WINDOW_HEIGHT = 600;
        SDL_RenderSetLogicalSize(gRenderer, WINDOW_WIDTH, WINDOW_HEIGHT);
        SDL_RenderSetViewport(gRenderer, NULL);
    }
    else if (strcmp(action, "1280x720") == 0)
    {
        SDL_SetWindowSize(gWindow, 1280, 720);
        WINDOW_WIDTH = 1280;
        WINDOW_HEIGHT = 720;
        SDL_RenderSetLogicalSize(gRenderer, WINDOW_WIDTH, WINDOW_HEIGHT);
        SDL_RenderSetViewport(gRenderer, NULL);
    }
    else if (strcmp(action, "full") == 0)
    {
        SDL_DisplayMode dm;
        SDL_GetCurrentDisplayMode(0, &dm);
        SDL_SetWindowSize(gWindow, dm.w, dm.h);
        SDL_SetWindowFullscreen(gWindow, SDL_WINDOW_FULLSCREEN);
        WINDOW_WIDTH = dm.w;
        WINDOW_HEIGHT = dm.h;
        SDL_RenderSetLogicalSize(gRenderer, WINDOW_WIDTH, WINDOW_HEIGHT);
        SDL_RenderSetViewport(gRenderer, NULL);
    }

    else if (strcmp(action, "window") == 0)
    {
        SDL_SetWindowFullscreen(gWindow, 0); // disables fullscreen
        SDL_SetWindowSize(gWindow, WINDOW_WIDTH, WINDOW_HEIGHT);
        SDL_RenderSetLogicalSize(gRenderer, WINDOW_WIDTH, WINDOW_HEIGHT);
    }
    SDL_PumpEvents();
    SDL_RenderPresent(gRenderer); // after unmounting the renderer, the resolution is switched and it is mounted again

    switch_resolution();
}

void free_page_data()
{
    for (int i = 0; i < page_data.dropdowns_count; i++) {
        free(page_data.dropdowns[i].label);
        free(page_data.dropdowns[i].font);
        for (int j = 0; j < page_data.dropdowns[i].option_count; j++) {
            free(page_data.dropdowns[i].options[j].text);
            free(page_data.dropdowns[i].options[j].action);
        }
        free(page_data.dropdowns[i].options);
    }

    for (int i = 0; i < page_data.labels_count; i++) {
        free(page_data.labels[i].text);
        free(page_data.labels[i].font);
    }

    for (int i = 0; i < page_data.buttons_count; i++) {
        free(page_data.buttons[i].text);
        free(page_data.buttons[i].font);
        free(page_data.buttons[i].action);
    }

    if (page_data.background) {
        SDL_DestroyTexture(page_data.background);
        page_data.background = NULL;
    }

    page_data.labels_count = 0;
    page_data.dropdowns_count = 0;
    page_data.buttons_count = 0;
}

void button_handler(const char* action)
{
    free_page_data();
    page_show = false;
}
void handle_page_event(int x, int y)
{
    for (int i = 0; i < page_data.dropdowns_count; ++i) {
        handle_dropdown_click(&page_data.dropdowns[i], x, y, handler_function);
    }
        for (int i = 0; i < page_data.buttons_count; ++i) {
        handle_button_click(&page_data.buttons[i], x, y, button_handler);
    }
}



