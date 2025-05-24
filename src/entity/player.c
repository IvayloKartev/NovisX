#include "player.h"
#include "../ui/map.h"
#include <string.h>
#include "../core/graphics.h"

static int x_pos = 0;
static int y_pos = 0;

static SDL_Texture* player_sprites[4];
static int current_sprite = 0;

bool show_player = false;

// 0 for up, 1 for down, 2 for right, 3 for left

void load_player_sprites(char* filename_up, char* filename_down, char* filename_right, char* filename_left)
{
    player_sprites[0] = graphics_load_texture_png(filename_up);
    player_sprites[1] = graphics_load_texture_png(filename_down);
    player_sprites[2] = graphics_load_texture_png(filename_right);
    player_sprites[3] = graphics_load_texture_png(filename_left);
}

void move_player(char* direction, int speed)
{
    if (strcmp(direction, "up") == 0)
    {
        y_pos -= speed;
        current_sprite = 0;
    }
    else if (strcmp(direction, "down") == 0)
    {
        y_pos += speed;
        current_sprite = 1;
    }
    else if (strcmp(direction, "right") == 0)
    {
        x_pos += speed;
        current_sprite = 2;
    }
    else if (strcmp(direction, "left") == 0)
    {
        x_pos -= speed;
        current_sprite = 3;
    }

    printf("PLAYER MOVED: (%d; %d)", x_pos, y_pos);

    move_camera(x_pos, y_pos);
}

void render_player()
{
    int player_width = 0;
    int player_height = 0;

    get_texture_dimensions(player_sprites[current_sprite], &player_width, &player_height);

    SDL_Rect dest = {400, 300, player_width, player_height};
    SDL_RenderCopy(gRenderer, player_sprites[current_sprite], NULL, &dest);
}


