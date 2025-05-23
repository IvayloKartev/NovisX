#include "graphics.h"
#include <stdlib.h>
#include "../ui/menu.h"
#include "../ui/map.h"
#include "../entity/player.h"
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <stdio.h>
#include <string.h>
#include "../screen/page.h"
#include "engine.h"

#define MAX_COMMANDS 512
#define MAX_CHARACTERS 10
#define MAX_DIALOGUES  20
#define MAX_NAME_LEN   64
#define MAX_TEXT_LEN   256

typedef struct {
    char name[MAX_NAME_LEN];
    char sprite[256];
} CharacterInfo;

typedef struct {
    char speaker[MAX_NAME_LEN];
    char text[MAX_TEXT_LEN];
} DialogueInfo;

typedef struct {
    CharacterInfo characters[MAX_CHARACTERS];
    int character_count;
    DialogueInfo dialogues[MAX_DIALOGUES];
    int dialogue_count;
} PanelData;

// enum of supported commands
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
    CMD_MAP_TILE,
    CMD_PANEL,
    CMD_PAGE
} CommandType;

// the object for one line of code - if new commands have more arguments, the maximum argument amount should be added
typedef struct {
    CommandType type;
    char arg1[256];
    char arg2[256];
    PanelData* arg3;
} Command;

static Command commands[512];
static int command_count = 0;
static int current_command = 0;

static SDL_Texture* previous_background = NULL;
static SDL_Texture* background = NULL;
static SDL_Texture* character[3];
static int character_count = 0;
static char current_speaker[64] = "";
static char current_text[512] = "";

static char choices[3][256];
static char choices_files[3][256];
int choice_index = 0;

static int letter_index = 0;
static Uint32 last_letter_time = 0;

static int alpha_step = 255;
static Uint32 alpha_step_time = 0;

//bool awaiting_choice = false;
bool map_loaded = false;
static bool background_should_fade = false;

static SDL_Texture* tile_textures[3];
static int tile_texture_current_index = 0;

static char direction_sprite_files[4][256];
int sprite_files_index = 0;

static SDL_Texture* text_bg = NULL;
static SDL_Texture* speaker_bg = NULL;

void add_command(CommandType type, const char* arg1, const char* arg2, PanelData* arg3) {
    if (command_count < MAX_COMMANDS) {
        Command cmd = {type, "", "", NULL};
        strncpy(cmd.arg1, arg1 ? arg1 : "", sizeof(cmd.arg1));
        strncpy(cmd.arg2, arg2 ? arg2 : "", sizeof(cmd.arg2));
        cmd.arg3 = arg3;
        commands[command_count++] = cmd;
    }
}

// function to get necessary data from the child nodes of a <panel> section
// it returns PanelData struct, that is then passed as arg3 to a CMD_PANEL command
// this is done because simply passing the whole XML node as an argument lead to segmentation faults, plus it is generally not a good idea to pass outside code around

PanelData* extract_panel_data(xmlNode* panelNode)
{
    xmlNode* curr;
    PanelData* panelData = malloc(sizeof(PanelData));
    if (!panelData) return NULL;
    memset(panelData, 0, sizeof(PanelData));

    panelData->character_count = 0;
    panelData->dialogue_count = 0;

    printf("Extracting panel data...\n");

    // iterating through the panel children
    for (curr = panelNode->children; curr; curr = curr->next) {
        // skip non-element nodes
        if (curr->type != XML_ELEMENT_NODE) continue;

        // sprite nodes
        if (strcmp((char*)curr->name, "character") == 0) {
            if (panelData->character_count >= MAX_CHARACTERS) continue;

            xmlChar* sprite = xmlGetProp(curr, (const xmlChar*)"sprite");
            xmlChar* name = xmlGetProp(curr, (const xmlChar*)"name");

            if (sprite) {
                printf("Loading sprite: %s\n", (char*)sprite);
                // !!!
                strcpy(panelData->characters[panelData->character_count].sprite, (char*)sprite);
                xmlFree(sprite);
            } else {
                strcpy(panelData->characters[panelData->character_count].sprite, "\0");
            }

            if (name) {
                strncpy(panelData->characters[panelData->character_count].name, (char*)name, MAX_NAME_LEN - 1);
                panelData->characters[panelData->character_count].name[MAX_NAME_LEN - 1] = '\0';
                xmlFree(name);
            } else {
                panelData->characters[panelData->character_count].name[0] = '\0';
            }

            printf("Extracted character: %s\n", panelData->characters[panelData->character_count].name);
            panelData->character_count++;
        }

        // dialogue nodes
        else if (strcmp((char*)curr->name, "dialogue") == 0) {
            for (xmlNode* dialogue = curr->children; dialogue; dialogue = dialogue->next) {
                if (dialogue->type != XML_ELEMENT_NODE) continue;
                if (strcmp((char*)dialogue->name, "say") == 0) {
                    if (panelData->dialogue_count >= MAX_DIALOGUES) continue;

                    xmlChar* speaker = xmlGetProp(dialogue, (const xmlChar*)"speaker");
                    xmlChar* text = xmlNodeGetContent(dialogue);

                    if (speaker) {
                        strncpy(panelData->dialogues[panelData->dialogue_count].speaker, (char*)speaker, MAX_NAME_LEN - 1);
                        panelData->dialogues[panelData->dialogue_count].speaker[MAX_NAME_LEN - 1] = '\0';
                        xmlFree(speaker);
                    } else {
                        panelData->dialogues[panelData->dialogue_count].speaker[0] = '\0';
                    }

                    if (text) {
                        strncpy(panelData->dialogues[panelData->dialogue_count].text, (char*)text, MAX_TEXT_LEN - 1);
                        panelData->dialogues[panelData->dialogue_count].text[MAX_TEXT_LEN - 1] = '\0';
                        xmlFree(text);
                    } else {
                        panelData->dialogues[panelData->dialogue_count].text[0] = '\0';
                    }

                    printf("Extracted dialogue: %s says: %s\n",
                           panelData->dialogues[panelData->dialogue_count].speaker,
                           panelData->dialogues[panelData->dialogue_count].text);
                    panelData->dialogue_count++;
                }
            }
        }
    }

    printf("Extraction complete.\n");
    return panelData;
}

// a recursive function that iterates through the XML tree and saves commands linearly
// despite using XML instead of the previous one-command-one-line language, the need to handle commands subsequently when developing a visual novel persisted
// might not be suitable for more complex logic

void parse_node(xmlNode* node) {
    for (xmlNode* curr = node; curr; curr = curr->next) {
        if (curr->type == XML_ELEMENT_NODE) {
            if (strcmp((char*)curr->name, "panel") == 0)
            {
                printf("About to process a panel: %s\n", (char*)curr->name);
                PanelData* panelData = extract_panel_data(curr);
                xmlChar* text_bg = xmlGetProp(curr, (const xmlChar*)"text-bg");
                xmlChar* speaker_bg = xmlGetProp(curr, (const xmlChar*)"speaker-bg");
                add_command(CMD_PANEL, (char*)text_bg, (char*)speaker_bg, panelData);
                continue;
            }
            else if (strcmp((char*)curr->name, "background") == 0) {
                xmlChar* file = xmlGetProp(curr, (const xmlChar*)"file");
                add_command(CMD_BACKGROUND, (char*)file, NULL, NULL);
                xmlFree(file);
            }
            else if (strcmp((char*)curr->name, "music") == 0)
            {
                xmlChar* file = xmlGetProp(curr, (const xmlChar*)"file");
                add_command(CMD_MUSIC, (char*)file, NULL, NULL);
                xmlFree(file);
            }
            else if (strcmp((char*)curr->name, "character") == 0)
            {
                xmlChar* sprite = xmlGetProp(curr, (const xmlChar*)"sprite");
                xmlChar* name = xmlGetProp(curr, (const xmlChar*)"name");
                add_command(CMD_CHARACTER, (char*)sprite, (char*)name, NULL);
                xmlFree(sprite);
                xmlFree(name);
            }
            else if (strcmp((char*)curr->name, "map") == 0)
            {
                xmlChar* name = xmlGetProp(curr, (const xmlChar*)"name");
                xmlChar* file = xmlGetProp(curr, (const xmlChar*)"file");
                add_command(CMD_MAP, (char*)name, (char*)file, NULL);
                xmlFree(name);
                xmlFree(file);
            }
            else if (strcmp((char*)curr->name, "tile") == 0) {
                xmlChar* file = xmlGetProp(curr, (const xmlChar*)"file");
                add_command(CMD_MAP_TILE, (char*)file, NULL, NULL);
                xmlFree(file);
            }
            else if (strcmp((char*)curr->name, "direction") == 0)
            {
                xmlChar* file = xmlGetProp(curr, (const xmlChar*)"file");
                strncpy(direction_sprite_files[sprite_files_index], (char*)file, sizeof(direction_sprite_files[sprite_files_index]));
                sprite_files_index++;
                if(sprite_files_index == 4)
                {
                    sprite_files_index = 0;
                    add_command(CMD_PLAYER_SPRITE, NULL, NULL, NULL);
                }
                xmlFree(file);
            }
            else if (strcmp((char*)curr->name, "say") == 0)
            {
                xmlChar* speaker = xmlGetProp(curr, (const xmlChar*)"speaker");
                xmlChar* text = xmlNodeGetContent(curr);
                add_command(CMD_SAY, (char*)speaker, (char*)text, NULL);
                xmlFree(speaker);
                xmlFree(text);
            }
            else if (strcmp((char*)curr->name, "choice") == 0)
            {
                xmlChar* text = xmlGetProp(curr, (const xmlChar*)"text");
                xmlChar* script = xmlGetProp(curr, (const xmlChar*)"script");
                add_command(CMD_CHOICE, (char*)text, (char*)script, NULL);
                xmlFree(text);
                xmlFree(script);
            }
            else if (strcmp((char*)curr->name, "menu") == 0) {
                xmlChar* script = xmlGetProp(curr, (const xmlChar*)"script");
                add_command(CMD_MENU, (char*)script, NULL, NULL);
                xmlFree(script);
            }
            else if (strcmp((char*)curr->name, "page") == 0)
            {
                xmlChar* script = xmlGetProp(curr, (const xmlChar*)"file");
                add_command(CMD_PAGE, (char*)script, NULL, NULL);
                xmlFree(script);
            }
            else if (strcmp((char*)curr->name, "label") == 0)
            {
                Label* label_data = assemble_label_xml(curr);
                printf("AFTER ASSEMBLY: %s %d %d\n", label_data->text, label_data->x_pos, label_data->y_pos);
                page_data.labels[page_data.labels_count] = *label_data;
                page_data.labels_count++;
                free(label_data);
            }
            else if (strcmp((char*)curr->name, "static") == 0)
            {
                printf("Entering a page: %s\n", (char*)curr->name);
                xmlChar* bg = xmlGetProp(curr, (const xmlChar*)"background");
                page_data.background = graphics_load_texture((char*)bg);
            }
            else if (strcmp((char*)curr->name, "dropdown") == 0)
            {
                Dropdown* dropdown_data = assemble_dropdown_xml(curr);
                page_data.dropdowns[page_data.dropdowns_count] = *dropdown_data;
                page_data.dropdowns_count++;
                free(dropdown_data);
            }
            else if (strcmp((char*)curr->name, "button") == 0)
            {
                printf("DETECTED BUTTON\n");
                Button* button_data = assemble_button_xml(curr);
                page_data.buttons[page_data.buttons_count] = *button_data;
                page_data.buttons_count++;
                free(button_data);
            }
        }
        // if it is not a panel, recursively go in deeper nodes
        if (strcmp((char*)curr->name, "panel") != 0)
        {
            parse_node(curr->children);
        }
    }
}

bool script_load_xml(const char* filename) {
    xmlDoc* doc = xmlReadFile(filename, NULL, 0);
    if (!doc) {
        printf("Could not parse XML file: %s\n", filename);
        return false;
    }

    // XML file should have ONLY ONE root element, for example <xml></xml> - everything else should reside within it
    xmlNode* root = xmlDocGetRootElement(doc);
    parse_node(root);

    xmlFreeDoc(doc);
    xmlCleanupParser();
    return true;
}

void character_unload()
{
    memset(current_speaker, 0 ,sizeof(current_speaker));
    memset(current_text, 0 ,sizeof(current_text));
}

void free_panel_data(PanelData* panelData) {
    if (panelData) {
        free(panelData);
    }
}

void script_unload() {
    if (background) SDL_DestroyTexture(background);
    for (int i = 0; i < character_count; i++)
    {
        if (character[i])  SDL_DestroyTexture(character[i]);
    }

    // PanelData is allocated dynamically, so iterating and freeing memory to prevent leaks
    for (int i = 0; i < command_count; i++)
    {
        if (commands[i].type == CMD_PANEL && commands[i].arg3)
        {
            free_panel_data(commands[i].arg3);
        }
    }
    character_count = 0;
    command_count = 0;
    awaiting_choice = false;
    memset(commands, 0, sizeof(commands));
    character_unload();
}

// going to the next command
void script_next() {
    if (current_command >= command_count) return;

    // it makes uses of sorta encapsulated current_command index
    Command* cmd = &commands[current_command++];
    switch (cmd->type) {
        // if an older texture exists, destroy it and render new
        case CMD_BACKGROUND:
            if (previous_background) SDL_DestroyTexture(previous_background);
            previous_background = background; // store current background
            background = graphics_load_texture(cmd->arg1); // load new one
            background_should_fade = true;
            alpha_step = 255;
            alpha_step_time = SDL_GetTicks();
            break;
        case CMD_CHARACTER:
            for(int i=0; i<character_count; i++)
            {
                if(character[i]) SDL_DestroyTexture(character[i]);
            }
            character[0] = graphics_load_texture(cmd->arg1);
            //character_count++;
            break;
        //set variables
        case CMD_SAY:
            strcpy(current_speaker, cmd->arg1);
            strcpy(current_text, cmd->arg2);
            break;
        case CMD_MUSIC:
            graphics_play_music(cmd->arg1);
            break;
        case CMD_CHOICE:
            strcpy(choices[choice_index], cmd->arg1);
            strcpy(choices_files[choice_index], cmd->arg2);
            awaiting_choice = true;
            choice_index++;
            break;
        case CMD_MENU:
            printf("Initializing menu: %s", cmd->arg1);
            init_menu("assets/menu.txt");
            break;
        case CMD_PAGE:
            printf("Initializing page: %s", cmd->arg1);
            load_page(cmd->arg1);
            break;
        case CMD_MAP:
            printf("MAP TO BE LOADED: %s\n", cmd->arg1);
            map_loaded = load_map(cmd->arg2);
            printf("MAP_LOADED: %b\n", map_loaded);
            break;
        case CMD_MAP_TILE:
            printf("%s\n", &(cmd->arg1));
            tile_textures[tile_texture_current_index] = graphics_load_texture(cmd->arg1);
            tile_texture_current_index++;
            printf("Assigned textures");
            break;
        case CMD_PLAYER_SPRITE:
            load_player_sprites(direction_sprite_files[0], direction_sprite_files[1], direction_sprite_files[2], direction_sprite_files[3]);
            show_player = true;
            break;
        case CMD_PANEL:
            if (cmd->arg3)
            {
                PanelData* panelData = (PanelData*)cmd->arg3;

                printf("Displaying panel with %d characters and %d dialogues.\n",
                       panelData->character_count, panelData->dialogue_count);

                // writing each character within a panel to the global array
                // since the script_render() function renders everything within the global arrays, everything inside <panel> element gets rendered all at once

                for (int i = 0; i < panelData->character_count; i++) {
                    printf("Character %d: %s\n", i + 1, panelData->characters[i].name);
                    if (panelData->characters[i].sprite) {
                        printf("Current sprite path: %s ; index: %d\n", panelData->characters[i].sprite, i);
                        character[i] = graphics_load_texture(panelData->characters[i].sprite);
                        character_count++;
                    }
                }

                for (int i = 0; i < panelData->dialogue_count; i++) {
                    printf("%s says: %s\n", panelData->dialogues[i].speaker, panelData->dialogues[i].text);
                    strcpy(current_speaker, panelData->dialogues[i].speaker);
                    strcpy(current_text, panelData->dialogues[i].text);
                }
            }
            if (cmd->arg1)
            {
                if(strcmp(cmd->arg1, "default") == 0)
                {
                    printf("About to draw textbg\n");
                    text_bg = create_transparent_rect_texture(WINDOW_WIDTH, 200, 200, 50, 50, 50);
                }
            }
            if (cmd->arg2)
            {
                if(strcmp(cmd->arg2, "default") == 0)
                {
                    printf("???");
                }
                printf("About to draw speakerbg\n");
                speaker_bg = graphics_resize_texture(graphics_load_texture(cmd->arg2), WINDOW_WIDTH, 40);
                //speaker_bg = create_transparent_rect_texture(800, 200, 200, 50, 50, 50);
            }
            //free(cmd->arg3);
            break;
        default:
            break;
    }

    letter_index = 0;
    last_letter_time = SDL_GetTicks();

}

void script_render() {

    SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255); // black background
    SDL_RenderClear(gRenderer);

    if (background_should_fade && previous_background)
    {
        graphics_draw_texture(background, 0, 0); // fully opaque
        SDL_SetTextureBlendMode(previous_background, SDL_BLENDMODE_BLEND);
        SDL_SetTextureAlphaMod(previous_background, alpha_step);
        graphics_draw_sized_texture(previous_background, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    }
    else if (background)
    {
        graphics_draw_sized_texture(background, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    }

    for (int i = 0; i < character_count; i++)
    {
        if (character[i]) graphics_scale_texture(character[i], 300 + (i * 100) - 100, 100 - 50 , 150);
    }

    if(map_loaded)
    {
        render_map(gRenderer, tile_textures);
    }
    if(show_player) render_player();
    if (awaiting_choice) {
        for(int i=0; i<3; i++)
        {
            graphics_draw_text(choices[i], 100, 450+50*i, NULL, 0, NULL, 255, 255, 255, 255);
        }
        //graphics_draw_text(choice1, 100, 450);
        //graphics_draw_text(choice2, 100, 500);
    } else {
        // attempts to display ordinary text as animated
        char partial[512];
        // partial string is used to render part of the text during each render, so animation appears
        // upon calling of script_update() partial will become longer and longer
        strncpy(partial, current_text, letter_index);
        // \0 to assign string
        partial[letter_index] = '\0';
        graphics_draw_text(current_speaker, 50, 450, NULL, 0, speaker_bg, 255, 0, 0, 0);
        graphics_draw_text(partial, 50, 490, NULL, 0, text_bg, 255, 255, 255, 255);
    }


}

// renders the selected script based on the selected index (files are obtained from the command in script_next())
void script_choice_click(int index)
{
    printf("Loading scene: %s\n", choices_files[index]);
    script_unload();
    script_load_xml(choices_files[index]);
    awaiting_choice = false;
    current_command = 0;
}

// if there are more letters to display, it increments the index, so on next render, a new letter appears
void script_update()
{
    if(alpha_step == 0)
    {
        background_should_fade = false;
        alpha_step = 255;
        if (previous_background) {
            SDL_DestroyTexture(previous_background);
            previous_background = NULL;
        }
    }
    if (letter_index < strlen(current_text))
    {
        if (SDL_GetTicks() - last_letter_time > 30)
        { // 30ms per letter
            letter_index++;
            last_letter_time = SDL_GetTicks();
        }
    }
    if (alpha_step >= 0 && background_should_fade) {
        if (SDL_GetTicks() - alpha_step_time > 10)
        { // 30ms per re-render
            alpha_step--;
            alpha_step_time = SDL_GetTicks();
        }
    }

}
