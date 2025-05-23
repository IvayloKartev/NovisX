#ifndef LABEL_H
#define LABEL_H

#include <SDL.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

typedef struct
{
    char* text;
    char* font;
    int font_size;
    int color[4];
    int x_pos;
    int y_pos;
    SDL_Texture* background;
} Label;

Label* assemble_label_xml(xmlNode* labelNode);
void render_label(char* text, char* font, int font_size, char* background_path, int* color, int x_pos, int y_pos);

#endif // LABEL_H
