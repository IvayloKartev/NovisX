#include "script.h"
#include "graphics.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "menu.h"
#include "map.h"
#include "player.h"
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
    CMD_MAP_TILE
} CommandType;

// the object for one line of code - if new commands have more arguments, the maximum argument amount should be added
typedef struct {
    CommandType type;
    char arg1[256];
    char arg2[256];
    char arg3[256];
    char arg4[256];
} Command;

static Command commands[512];
static int command_count = 0;
static int current_command = 0;

static SDL_Texture* previous_background = NULL;
static SDL_Texture* background = NULL;
static SDL_Texture* character = NULL;
static char current_speaker[64] = "";
static char current_text[512] = "";

static char choice1[256];
static char choice2[256];
static char file1[256];
static char file2[256];

static int letter_index = 0;
static Uint32 last_letter_time = 0;

static int alpha_step = 255;
static Uint32 alpha_step_time = 0;

bool character_say[512]; //tracks where the character image should be displayed together with the text
bool awaiting_choice = false;
bool map_loaded = false;
static bool background_should_fade = false;

static SDL_Texture* tile_textures[3];
static int tile_texture_current_index = 0;

bool script_load(const char* filename) {
    FILE* f = fopen(filename, "r");
    if (!f) return false;

    char line[512];
    // going through every command in the script file
    while (fgets(line, sizeof(line), f)) {
        Command cmd = {CMD_NONE, "", ""};

        // checks if first n symbols form a known command and adjusts variables accordingly
        if (strncmp(line, "background", 10) == 0) {
            cmd.type = CMD_BACKGROUND;
            sscanf(line, "background %s", cmd.arg1);
        } else if (strncmp(line, "music", 5) == 0) {
            cmd.type = CMD_MUSIC;
            sscanf(line, "music %s", cmd.arg1);
        } else if (strncmp(line, "character", 9) == 0) {
            cmd.type = CMD_CHARACTER;
            printf("Current line: %s\n", line);
            if(strchr(line, '+') != NULL)  //if there is + at the end of the line, the text should be rendered with the image
            {
                printf("Index %d requires text rendering", command_count);
                character_say[command_count] = true;
                sscanf(line, "character %s +", cmd.arg1);
            }
            else sscanf(line, "character %s", cmd.arg1);
        }
        else if (strncmp(line, "say", 3) == 0) {
            cmd.type = CMD_SAY;
            char speaker[64], text[512];
            sscanf(line, "say \"%[^\"]\" \"%[^\"]\"", speaker, text);
            strcpy(cmd.arg1, speaker);
            strcpy(cmd.arg2, text);
        }
        else if (strncmp(line, "choice", 6) == 0) {
            cmd.type = CMD_CHOICE;
            sscanf(line, "choice \"%[^\"]\" \"%[^\"]\" \"%[^\"]\" \"%[^\"]\"", cmd.arg1, cmd.arg2, cmd.arg3, cmd.arg4);
        }
        else if (strncmp(line, "menu", 4) == 0)
        {
            cmd.type = CMD_MENU;
            sscanf(line, "menu %s", cmd.arg1);
            printf("Line: %s\n", line);
        }

        else if (strncmp(line, "map_init", 8) == 0)
        {
            cmd.type = CMD_MAP;
            sscanf(line, "map_init %s", cmd.arg1);
            printf("Line: %s\n", line);
        }

        else if (strncmp(line, "map_tile", 8) == 0)
        {
            cmd.type = CMD_MAP_TILE;
            sscanf(line, "map_tile %s", cmd.arg1);
            printf("Map tile path: %s\n", cmd.arg1);
        }
        else if (strncmp(line, "player_sprite", 13) == 0)
        {
            cmd.type = CMD_PLAYER_SPRITE;
            sscanf(line, "player_sprite %s : %s : %s : %s", cmd.arg1, cmd.arg2, cmd.arg3, cmd.arg4);
        }

        commands[command_count++] = cmd;
    }

    fclose(f);
    return true;
}

void script_unload() {
    if (background) SDL_DestroyTexture(background);
    if (character) SDL_DestroyTexture(character);
}

void character_unload()
{
    memset(current_speaker,0,sizeof(current_speaker));
    memset(current_text,0,sizeof(current_text));
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
            if (character) SDL_DestroyTexture(character);
            character = graphics_load_texture(cmd->arg1);
            printf("Current index %d", current_command);
            // checking if text should also be rendered in the current frame
            if(character_say[current_command-1])
            {
                printf("Detected skip!!!\n");
                strcpy(current_speaker, commands[current_command].arg1);
                strcpy(current_text, commands[current_command].arg2);
                //skip the next command to prevent re-rendering of the text
                current_command++;

            }
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
            strcpy(choice1, cmd->arg1);
            strcpy(file1, cmd->arg2);
            strcpy(choice2, cmd->arg3);
            strcpy(file2, cmd->arg4);
            awaiting_choice = true;
            break;
        case CMD_MENU:
            printf("Initializing menu: %s", cmd->arg1);
            init_menu("assets/menu.txt");
            break;
        case CMD_MAP:
            printf("MAP_LOADED: %b\n", map_loaded);
            map_loaded = load_map(cmd->arg1);
            printf("MAP_LOADED: %b\n", map_loaded);
            break;
        case CMD_MAP_TILE:
            printf("%s\n", &(cmd->arg1));
            tile_textures[tile_texture_current_index] = graphics_load_texture(cmd->arg1);
            tile_texture_current_index++;
            printf("Assigned textures");
            break;
        case CMD_PLAYER_SPRITE:
            load_player_sprites(cmd->arg1, cmd->arg2, cmd->arg3, cmd->arg4);
            show_player = true;
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
        graphics_draw_texture(previous_background, 0, 0);
    }
    else if (background)
    {
        graphics_draw_texture(background, 0, 0);
    }
    if (character) graphics_draw_texture(character, 300, 100);

    if(map_loaded)
    {
        //printf("Will print a map!");
        render_map(gRenderer, tile_textures);
    }
    if(show_player) render_player();
    if (awaiting_choice) {
        graphics_draw_text(choice1, 100, 450);
        graphics_draw_text(choice2, 100, 500);
    } else {
        // attempts to display ordinary text as animated
        char partial[512];
        // partial string is used to render part of the text during each render, so animation appears
        // upon calling of script_update() partial will become longer and longer
        strncpy(partial, current_text, letter_index);
        // \0 to assign string
        partial[letter_index] = '\0';
        graphics_draw_text(current_speaker, 50, 450);
        graphics_draw_text(partial, 50, 500);
    }


}

// renders the selected script based on the selected index (files are obtained from the command in script_next())
void script_choice_click(int index)
{
    if (index == 0)
    {
        script_unload();
        script_load(file1);
    }
    else
    {
        script_unload();
        script_load(file2);
    }
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
