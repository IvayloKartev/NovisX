#include "player.h"
#include "../ui/map.h"
#include <string.h>
#include "../core/graphics.h"
#include "../core/parser.h"
#include <math.h>
#include "events.h"

static int x_pos = 0;
static int y_pos = 0;

static int player_height = 0;
static int player_width = 0;

static float velocity_x = 0;
static float velocity_y = 0;
static float gravity_accel = 0.5f;
static float max_gravity_speed = 8.0f;
static float move_accel = 1.0f;
static float max_horizontal_speed = 8.0f;
static float jump_velocity = -10.0f;

static SDL_Texture* player_sprites[4];
static int current_sprite = 0;

bool show_player = false;
bool has_gravity = false;
char gravity_direction[8] = "none"; // "up", "down", "left", "right"

bool can_jump = false;
static bool is_grounded = false;


void load_player_sprites(char* filename_up, char* filename_down, char* filename_right, char* filename_left)
{
    player_sprites[0] = graphics_load_texture_png(filename_up);
    player_sprites[1] = graphics_load_texture_png(filename_down);
    player_sprites[2] = graphics_load_texture_png(filename_right);
    player_sprites[3] = graphics_load_texture_png(filename_left);
}

void enable_player_jump(float height)
{
    can_jump = true;
    jump_velocity = -sqrtf(2 * gravity_accel * height); // physics formula v = sqrt(2gh)
}

bool is_tile_collidable(int x, int y, char** map)
{
    if (x < 0 || y < 0 || x >= 300 || y >= 300) return true;

    char tile_type = map[y][x];
    int texture_index;

    if (tile_type >= '0' && tile_type <= '9') {
        texture_index = tile_type - '0';
    } else if (tile_type >= 'A' && tile_type <= 'Z') {
        texture_index = 10 + (tile_type - 'A');
    } else {
        return true;
    }

    return tile_has_collision[texture_index];
}

void set_player_gravity(const char* dir)
{
    has_gravity = true;
    strncpy(gravity_direction, dir, sizeof(gravity_direction));
    gravity_direction[sizeof(gravity_direction) - 1] = '\0';
    velocity_x = 0;
    velocity_y = 0;
}

void player_jump()
{
    printf("IS GROUNDED: %b\n", is_grounded);
    if (can_jump && is_grounded)
    {
        velocity_y = jump_velocity;
        is_grounded = false;
    }
}

void move_player(char* direction, int speed)
{

    int target_x = x_pos;
    int target_y = y_pos;

    if (strcmp(direction, "left") == 0)
    {
        if(has_gravity) velocity_x -= move_accel*speed;
        else target_x -= speed;
        current_sprite = 3;
    }
    else if (strcmp(direction, "right") == 0)
    {
        if(has_gravity) velocity_x += move_accel*speed;
        else target_x += speed;
        current_sprite = 2;
    }
    else if (strcmp(direction, "down") == 0)
    {
        if(has_gravity) velocity_y += move_accel*speed;
        else target_y += speed;
        current_sprite = 1;
    }
    else if (strcmp(direction, "up") == 0)
    {
        if(has_gravity) velocity_y -= move_accel*speed;
        else target_y -= speed;
        current_sprite = 0;
    }

    int tile_x = (target_x + WINDOW_WIDTH / 2) / 32;
    int tile_y = (target_y + WINDOW_HEIGHT / 2) / 32;

    if (!is_tile_collidable(tile_x, tile_y, current_loaded_map))
    {
        x_pos = target_x;
        y_pos = target_y;
    }

    if(!has_gravity) move_camera(x_pos, y_pos);
}

// this function is used only when gravity is on, so it returns if it is turned off
void update_player()
{
    if (!has_gravity || strcmp(gravity_direction, "none") == 0)
        return;

    // apply gravity
    velocity_y += gravity_accel;
    if (velocity_y > max_gravity_speed) velocity_y = max_gravity_speed;

    // limit horizontal speed
    if (velocity_x > max_horizontal_speed) velocity_x = max_horizontal_speed;
    if (velocity_x < -max_horizontal_speed) velocity_x = -max_horizontal_speed;

    float new_x = x_pos + velocity_x;
    float new_y = y_pos + velocity_y;

    // TILE CORNER CHECKS
    // world-to-tile conversion: add camera offset, divide by tile size
    int tile_x_left   = (new_x + WINDOW_WIDTH / 2) / 32;
    int tile_x_right  = (new_x + WINDOW_WIDTH / 2 + player_width - 1) / 32;

    int tile_y_top    = (new_y + WINDOW_HEIGHT / 2) / 32;
    int tile_y_bottom = (new_y + WINDOW_HEIGHT / 2 + player_height - 1) / 32;

    // Y collision check
    bool y_collides = false;

    int y_collision_tiles[2][2] =
    {
        { tile_x_left, tile_y_bottom },
        { tile_x_right, tile_y_bottom }
    };

    for (int i = 0; i < 2; i++)
    {
        int tx = y_collision_tiles[i][0];
        int ty = y_collision_tiles[i][1];
        int tile_id = get_tile_at(tx, ty, current_loaded_map);

        if (is_tile_collidable(tx, ty, current_loaded_map))
        {
            y_collides = true;

            if (tile_id >= 0 && tile_id < 10 && tile_collision_scripts[tile_id])
            {
                script_unload();
                script_load_xml(tile_collision_scripts[tile_id]);
            }
        }
    }

    if (!y_collides)
    {
        y_pos = (int)new_y;
        is_grounded = false;
    }
    else
    {
        velocity_y = 0;
    }

    // X collision check
    tile_y_top    = (y_pos + WINDOW_HEIGHT / 2) / 32;
    tile_y_bottom = (y_pos + WINDOW_HEIGHT / 2 + player_height - 1) / 32;

    tile_x_left  = (new_x + WINDOW_WIDTH / 2) / 32;
    tile_x_right = (new_x + WINDOW_WIDTH / 2 + player_width - 1) / 32;

    bool x_collides = false;

    int x_collision_tiles[4][2] = {
        { tile_x_left, tile_y_top },
        { tile_x_left, tile_y_bottom },
        { tile_x_right, tile_y_top },
        { tile_x_right, tile_y_bottom }
    };

    for (int i = 0; i < 4; i++)
    {
        int tx = x_collision_tiles[i][0];
        int ty = x_collision_tiles[i][1];
        int tile_id = get_tile_at(tx, ty, current_loaded_map);

        if (is_tile_collidable(tx, ty, current_loaded_map))
        {
            x_collides = true;

            // execute script if tile has one
            if (tile_id >= 0 && tile_id < MAX_TILE_TYPES && tile_collision_scripts[tile_id])
            {
                script_unload();
                script_load_xml(tile_collision_scripts[tile_id]);
            }
        }
    }

    if (!x_collides)
    {
        x_pos = (int)new_x;
    }
    else
    {
        velocity_x = 0;
    }


    // check ground
    int foot_left  = (x_pos + WINDOW_WIDTH / 2) / 32;
    int foot_right = (x_pos + WINDOW_WIDTH / 2 + player_width - 1) / 32;
    int foot_y     = (y_pos + WINDOW_HEIGHT / 2 + player_height) / 32;

    is_grounded =
        is_tile_collidable(foot_left, foot_y, current_loaded_map) ||
        is_tile_collidable(foot_right, foot_y, current_loaded_map);

    // apply friction when grounded
    if (is_grounded)
    {
        velocity_x *= 0.8f;
        if (fabs(velocity_x) < 0.1f) velocity_x = 0;
    }

    move_camera(x_pos, y_pos);
}


void render_player()
{

    get_texture_dimensions(player_sprites[current_sprite], &player_width, &player_height);

    SDL_Rect dest = {WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, player_width, player_height};
    SDL_RenderCopy(gRenderer, player_sprites[current_sprite], NULL, &dest);
}

void switch_resolution()
{
    printf("BEFORE: %d\n", y_pos);
    float change = (600.0f/720.0f);

    y_pos = (int)(change*y_pos);
    printf("AFTER: %d\n", y_pos);
}
