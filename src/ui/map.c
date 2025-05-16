#include "map.h"
#include "../core/graphics.h"

#define MAP_SIZE 299
#define MAP_WIDTH 299
#define MAP_HEIGHT 299
#define TILE_SIZE 32
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#include <SDL2/SDL.h>

static char terrain_map[MAP_SIZE][MAP_SIZE];
int cam_x = 0, cam_y = 0;
//static SDL_Texture* tile_textures[3]; // to be allocated dynamically

bool load_map(char* filename)
{
    FILE* f = fopen(filename, "r");
    if (!f) return false;

    char line[299];
    int row_index = 0;
    // going through every line in the map file
    while (fgets(line, sizeof(line), f))
    {
        int len = strlen(line);
        // remove the newline character if present
        if (line[len - 1] == '\n') {
            line[len - 1] = '\0';
            len--;
        }

        for (int i = 0; i < 299; i++)  // Only copy actual data length
        {
            terrain_map[row_index][i] = line[i];
            //if(i==2) printf("SECOND LINE %s", line);
        }
        row_index++;
        //if(row_index == 2) break;

        // check to avoid overflowing the map
        if (row_index >= MAP_SIZE) break;
    }
    fclose(f);
    return true;

}

// function to keep camera within bounds
int clamp(int value, int _min, int _max)
{
    if (value < _min) return _min;
    if (value > _max) return _max;
    return value;
}

void move_camera(int player_x, int player_y)
{
    cam_x = clamp((player_x - (SCREEN_WIDTH / 2)), 0, (MAP_SIZE * TILE_SIZE) - SCREEN_WIDTH);
    cam_y = clamp((player_y - (SCREEN_HEIGHT / 2)), 0, (MAP_SIZE * TILE_SIZE) - SCREEN_HEIGHT);
    cam_x = player_x;
    cam_y = player_y;
    printf("Current camera coordinates: (%d; %d)\n", cam_x, cam_y);
}

void render_map(SDL_Renderer* renderer, SDL_Texture* textures[3])
{
    //printf("Preparing to render\n");
    
    int start_col = cam_x / TILE_SIZE;
    int start_row = cam_y / TILE_SIZE;
    int end_col = (cam_x + SCREEN_WIDTH) / TILE_SIZE;
    int end_row = (cam_y + SCREEN_HEIGHT) / TILE_SIZE;

    if (start_col < 0) start_col = 0;
    if (start_row < 0) start_row = 0;
    if (end_col >= MAP_WIDTH) end_col = MAP_WIDTH - 1;
    if (end_row >= MAP_HEIGHT) end_row = MAP_HEIGHT - 1;

    //printf("Start: col=%d, row=%d | End: col=%d, row=%d\n", start_col, start_row, end_col, end_row);

    for (int row = start_row; row <= end_row; row++)
    {
        if (row < 0 || row >= MAP_HEIGHT) continue;
       // printf("Rendering row %d\n", row);

        for (int col = start_col; col <= end_col; col++)
        {
            if (col < 0 || col >= MAP_WIDTH) continue;
         //   printf("Rendering column %d\n", col);

            char tile_type = terrain_map[row][col];
            int texture_index = tile_type - '0';

            if (texture_index < 0 || texture_index >= 3) {
                //printf("Invalid texture index: %d\n", texture_index);
                continue;
            }

            SDL_Rect dest = {
                (col * TILE_SIZE) - cam_x,
                (row * TILE_SIZE) - cam_y,
                TILE_SIZE, TILE_SIZE
            };
           // printf("Rendering texture at row %d, col %d, texture index %d\n", row, col, texture_index);
            SDL_RenderCopy(renderer, textures[texture_index], NULL, &dest);
        }
    }

    //printf("Current cam coords: x: %d/ y: %d", cam_x, cam_y);
}

// TODO
void assign_textures(SDL_Texture** textures, int textures_length)
{
    for(int i=0; i<textures_length; i++)
    {
        //tile_textures[i] = textures[i];
    }
}



