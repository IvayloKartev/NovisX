#ifndef PAGE_H
#define PAGE_H

#include "../ui/components/components.h"

typedef struct
{
    SDL_Texture* background;
    Label labels[10];
    int labels_count;
    Dropdown dropdowns[2];
    int dropdowns_count;
    Button buttons[2];
    int buttons_count;
} PageData;

void load_page(char* filename);
void render_page();
void handle_page_event(int x, int y);

extern PageData page_data;
#endif // PAGE_H
