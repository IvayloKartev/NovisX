#include "button.h"
#include "../../core/graphics.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

Button* assemble_button_xml(xmlNode* buttonNode) {
    printf("Assembling button...\n");
    Button* button = malloc(sizeof(Button));
    if (!button) return NULL;
    memset(button, 0, sizeof(Button));

    button->text = (char*)xmlGetProp(buttonNode, (const xmlChar*)"text");
    button->action = (char*)xmlGetProp(buttonNode, (const xmlChar*)"action");
    button->x_pos = atoi((char*)xmlGetProp(buttonNode, (const xmlChar*)"x"));
    button->y_pos = atoi((char*)xmlGetProp(buttonNode, (const xmlChar*)"y"));
    button->width = atoi((char*)xmlGetProp(buttonNode, (const xmlChar*)"width"));
    button->height = atoi((char*)xmlGetProp(buttonNode, (const xmlChar*)"height"));

    button->color[0] = 255;
    button->color[1] = 255;
    button->color[2] = 255;
    button->color[3] = 255;

    xmlNode* curr;
    for (curr = buttonNode->children; curr; curr = curr->next) {
        if (curr->type != XML_ELEMENT_NODE) continue;

        if (strcmp((char*)curr->name, "font") == 0) {
            button->font = (char*)xmlGetProp(curr, (const xmlChar*)"path");
            button->font_size = atoi((char*)xmlGetProp(curr, (const xmlChar*)"size"));
        } else if (strcmp((char*)curr->name, "color") == 0) {
            button->color[0] = atoi((char*)xmlGetProp(curr, (const xmlChar*)"r"));
            button->color[1] = atoi((char*)xmlGetProp(curr, (const xmlChar*)"g"));
            button->color[2] = atoi((char*)xmlGetProp(curr, (const xmlChar*)"b"));
            button->color[3] = atoi((char*)xmlGetProp(curr, (const xmlChar*)"a"));
        }
    }

    return button;
}

void render_button(Button* button) {
    SDL_SetRenderDrawColor(gRenderer, 100, 100, 100, 255);
    SDL_Rect rect = { button->x_pos, button->y_pos, button->width, button->height };
    SDL_RenderFillRect(gRenderer, &rect);

    // Draw text centered in the button
    graphics_draw_text(button->text,
                       button->x_pos + button->width / 2,
                       button->y_pos + button->height / 2,
                       button->font,
                       button->font_size,
                       button->background,
                       button->color[3],
                       button->color[0],
                       button->color[1],
                       button->color[2]);
}

void handle_button_click(Button* button, int mouse_x, int mouse_y, void (*action_handler)(const char* action)) {
    if (mouse_x >= button->x_pos && mouse_x <= button->x_pos + button->width &&
        mouse_y >= button->y_pos && mouse_y <= button->y_pos + button->height) {
        action_handler(button->action);
    }
}
