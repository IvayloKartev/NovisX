#include "label.h"
#include "../../core/graphics.h"
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <stdlib.h>

Label* assemble_label_xml(xmlNode* labelNode)
{
    printf("Assembling label: %s\n",(char*)xmlGetProp(labelNode, (const xmlChar*)"text"));
    xmlNode* curr;
    Label* label_data = malloc(sizeof(Label));
    if (!label_data) return NULL;
    memset(label_data, 0, sizeof(Label));

    //first initalizing data from the props of the root
    label_data->text = (char*)xmlGetProp(labelNode, (const xmlChar*)"text");
    label_data->x_pos = atoi((char*)xmlGetProp(labelNode, (const xmlChar*)"x"));
    label_data->y_pos = atoi((char*)xmlGetProp(labelNode, (const xmlChar*)"y"));
    label_data->background = graphics_load_texture((char*)xmlGetProp(labelNode, (const xmlChar*)"bg"));

    //setting default values to the rest
    label_data->color[0] = 0;
    label_data->color[1] = 0;
    label_data->color[2] = 0;
    label_data->color[3] = 0;
    label_data->font = NULL;
    label_data->font_size = 0;

    printf("Extracting label data... %d\n", label_data->color[0]);

    // iterating through the label children
    for (curr = labelNode->children; curr; curr = curr->next) {
        // skip non-element nodes
        if (curr->type != XML_ELEMENT_NODE) continue;

        if (strcmp((char*)curr->name, "color") == 0)
        {
            label_data->color[0] = atoi((char*)xmlGetProp(curr, (const xmlChar*)"r"));
            label_data->color[1] = atoi((char*)xmlGetProp(curr, (const xmlChar*)"g"));
            label_data->color[2] = atoi((char*)xmlGetProp(curr, (const xmlChar*)"b"));
            label_data->color[3] = atoi((char*)xmlGetProp(curr, (const xmlChar*)"a"));
        }
        else if (strcmp((char*)curr->name, "font") == 0)
        {
            label_data->font = (char*)xmlGetProp(curr, (const xmlChar*)"file");
            label_data->font_size = atoi((char*)xmlGetProp(curr, (const xmlChar*)"size"));
        }
    }
    return label_data;
}

void render_label(char* text, char* font, int font_size, char* background_path, int* color, int x_pos, int y_pos)
{
    SDL_Texture* bg = NULL;
    if(background_path) bg = graphics_load_texture(background_path);
    graphics_draw_text(text, x_pos, y_pos, font, font_size, bg, color[3], color[0], color[1], color[2]);
}

