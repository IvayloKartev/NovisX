#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

typedef enum {
    CMD_NONE,
    CMD_BACKGROUND,
    CMD_MUSIC,
    CMD_CHARACTER,
    CMD_SAY,
    CMD_CHOICE,
    CMD_MENU,
    CMD_MAP,
    CMD_PLAYER_SPRITE,
    CMD_MAP_TILE
} CommandType;

void add_command(CommandType type, const char* arg1, const char* arg2);
void parse_node(xmlNode* node);
bool script_load_xml(const char* filename);
void script_unload();
void character_unload();

void script_next();
void script_render();
void script_update();
void script_choice_click(int index);

extern bool tile_has_collision[36];
extern char** current_loaded_map;

#endif
