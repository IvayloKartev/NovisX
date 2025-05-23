#include "dropdown.h"
#include "../../core/graphics.h"
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static DropdownType parse_dropdown_type(const char* type_str) {
    if (strcmp(type_str, "resolution") == 0) {
        return DROPDOWN_TYPE_RESOLUTION;
    }
    return DROPDOWN_TYPE_RESOLUTION;
}

Dropdown* assemble_dropdown_xml(xmlNode* dropdownNode) {
    printf("Assembling dropdown...\n");
    Dropdown* dropdown = malloc(sizeof(Dropdown));
    if (!dropdown) return NULL;
    memset(dropdown, 0, sizeof(Dropdown));

    dropdown->label = (char*)xmlGetProp(dropdownNode, (const xmlChar*)"label");
    dropdown->type = parse_dropdown_type((char*)xmlGetProp(dropdownNode, (const xmlChar*)"type"));

    dropdown->x_pos = 100; // Default pos
    dropdown->y_pos = 100;

    dropdown->color[0] = 255;
    dropdown->color[1] = 255;
    dropdown->color[2] = 255;
    dropdown->color[3] = 255;

    dropdown->is_open = 0;
    dropdown->animation_progress = 0.0f;

    xmlNode* curr;
    for (curr = dropdownNode->children; curr; curr = curr->next) {
        if (curr->type != XML_ELEMENT_NODE) continue;

        if (strcmp((char*)curr->name, "font") == 0) {
            dropdown->font = (char*)xmlGetProp(curr, (const xmlChar*)"path");
            dropdown->font_size = atoi((char*)xmlGetProp(curr, (const xmlChar*)"size"));
        } else if (strcmp((char*)curr->name, "color") == 0) {
            dropdown->color[0] = atoi((char*)xmlGetProp(curr, (const xmlChar*)"r"));
            dropdown->color[1] = atoi((char*)xmlGetProp(curr, (const xmlChar*)"g"));
            dropdown->color[2] = atoi((char*)xmlGetProp(curr, (const xmlChar*)"b"));
            dropdown->color[3] = atoi((char*)xmlGetProp(curr, (const xmlChar*)"a"));
        } else if (strcmp((char*)curr->name, "options") == 0) {
            // count <option> tags
            int count = 0;
            xmlNode* opt;
            for (opt = curr->children; opt; opt = opt->next)
                if (opt->type == XML_ELEMENT_NODE && strcmp((char*)opt->name, "option") == 0) count++;

            dropdown->options = malloc(sizeof(DropdownOption) * count);
            dropdown->option_count = count;

            int i = 0;
            for (opt = curr->children; opt; opt = opt->next) {
                if (opt->type != XML_ELEMENT_NODE || strcmp((char*)opt->name, "option") != 0) continue;

                dropdown->options[i].action = (char*)xmlGetProp(opt, (const xmlChar*)"action");
                dropdown->options[i].text = (char*)xmlNodeGetContent(opt);
                i++;
            }
        }
    }

    return dropdown;
}

void render_dropdown(Dropdown* dropdown) {
    int item_height = dropdown->font_size + 10;

    // Render label
    graphics_draw_text(dropdown->label, dropdown->x_pos, dropdown->y_pos, dropdown->font,
                       dropdown->font_size, dropdown->background, dropdown->color[3],
                       dropdown->color[0], dropdown->color[1], dropdown->color[2]);

    // Render selected state / collapsed box
    graphics_draw_text("[+]", dropdown->x_pos + 200, dropdown->y_pos, dropdown->font,
                       dropdown->font_size, NULL, dropdown->color[3],
                       dropdown->color[0], dropdown->color[1], dropdown->color[2]);

    // Animate open dropdown
    if (dropdown->is_open && dropdown->animation_progress < 1.0f)
        dropdown->animation_progress += 0.1f;
    else if (!dropdown->is_open && dropdown->animation_progress > 0.0f)
        dropdown->animation_progress -= 0.1f;

    int visible_options = (int)(dropdown->option_count * dropdown->animation_progress);

    for (int i = 0; i < visible_options; i++) {
        int y = dropdown->y_pos + (i + 1) * item_height;
        graphics_draw_text(dropdown->options[i].text, dropdown->x_pos + 20, y, dropdown->font,
                           dropdown->font_size, NULL, dropdown->color[3],
                           dropdown->color[0], dropdown->color[1], dropdown->color[2]);
    }
}

void handle_dropdown_click(Dropdown* dropdown, int mouse_x, int mouse_y, void (*action_handler)(const char* action)) {
    int item_height = dropdown->font_size + 10;
    int box_x = dropdown->x_pos + 200;
    int box_y = dropdown->y_pos;

    // toggle open/close if clicked main box
    if (mouse_x >= box_x && mouse_x <= box_x + 30 && mouse_y >= box_y && mouse_y <= box_y + item_height) {
        dropdown->is_open = !dropdown->is_open;
        return;
    }

    // if open, check if clicked any option
    if (dropdown->is_open) {
        for (int i = 0; i < dropdown->option_count; i++) {
            int y = dropdown->y_pos + (i + 1) * item_height;
            if (mouse_x >= dropdown->x_pos && mouse_x <= dropdown->x_pos + 200 &&
                mouse_y >= y && mouse_y <= y + item_height) {
                dropdown->is_open = 0;
                dropdown->animation_progress = 0.0f;
                action_handler(dropdown->options[i].action);
                break;
            }
        }
    }
}

