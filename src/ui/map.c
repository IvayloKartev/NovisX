#include "map.h"
#include "../core/graphics.h"

#define MAP_SIZE 300
#define MAP_WIDTH 300
#define MAP_HEIGHT 300
#define TILE_SIZE 32

#include <SDL2/SDL.h>

static char terrain_map[MAP_SIZE][MAP_SIZE];
int cam_x = 0, cam_y = 0;
//static SDL_Texture* tile_textures[3]; // to be allocated dynamically

bool load_map(char* filename)
{
    FILE* f = fopen(filename, "r");
    if (!f) return false;

    char line[300];
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

        for (int i = 0; i < 300; i++)  // Only copy actual data length
        {
            terrain_map[row_index][i] = line[i];
            //if(i==2) printf("SECOND LINE %s", line);
        }
        row_index++;
        //if(row_index == 2) break;

        // check to avoid overflowing the map
        if (row_index >= MAP_SIZE) break;
    }
    printf("ROW_COUNT: %d\n", row_index);
    fclose(f);
    return true;

}

char** load_2D_map(char* filename)
{
    FILE* f = fopen(filename, "r");
    if (!f) return NULL;

    char** map = malloc(MAP_SIZE * sizeof(char*));
    if (!map) {
        fclose(f);
        return NULL;
    }

    char line[MAP_SIZE];
    int row_index = 0;

    while (fgets(line, sizeof(line), f) && row_index < MAP_SIZE)
    {
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
            len--;
        }

        // Allocate memory for the row and copy the line
        map[row_index] = malloc(MAP_SIZE * sizeof(char));
        memset(map[row_index], 0, MAP_SIZE);  // optional: clear memory
        strncpy(map[row_index], line, MAP_SIZE - 1);  // avoid overflow

        row_index++;
    }

    fclose(f);
    return map;
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
    cam_x = clamp((player_x - (WINDOW_WIDTH / 2)), 0, (MAP_SIZE * TILE_SIZE) - WINDOW_WIDTH);
    cam_y = clamp((player_y - (WINDOW_HEIGHT / 2)), 0, (MAP_SIZE * TILE_SIZE) - WINDOW_HEIGHT);
    cam_x = player_x;
    cam_y = player_y;
    //printf("Current camera coordinates: (%d; %d)\n", cam_x, cam_y);
}

void render_map(SDL_Renderer* renderer, SDL_Texture* textures[36])
{
    //printf("Preparing to render\n");

    int start_col = cam_x / TILE_SIZE;
    int start_row = cam_y / TILE_SIZE;
    int end_col = (cam_x + WINDOW_WIDTH) / TILE_SIZE;
    int end_row = (cam_y + WINDOW_HEIGHT) / TILE_SIZE;

    if (start_col < 0) start_col = 0;
    if (start_row < 0) start_row = 0;
    if (end_col >= MAP_WIDTH) end_col = MAP_WIDTH - 1;
    if (end_row >= MAP_HEIGHT) end_row = MAP_HEIGHT - 1;

    for (int row = start_row; row <= end_row; row++)
    {
        if (row < 0 || row >= MAP_HEIGHT) continue;
       // printf("Rendering row %d\n", row);
        for (int col = start_col; col <= end_col; col++)
        {
            if (col < 0 || col >= MAP_WIDTH) continue;
         //   printf("Rendering column %d\n", col);

            char tile_type = terrain_map[row][col];
            int texture_index = 0;
            //else texture_index = 10;
            if (tile_type >= '0' && tile_type <= '9')
            {
                texture_index = tile_type - '0';
            }
            else if (tile_type >= 'A' && tile_type <= 'Z')
            {
                texture_index = 10 + (tile_type - 'A');
            }

            SDL_Rect dest = {
                (col * TILE_SIZE) - cam_x,
                (row * TILE_SIZE) - cam_y - 16,
                TILE_SIZE, TILE_SIZE
            };
           // printf("Rendering texture at row %d, col %d, texture index %d\n", row, col, texture_index);
            SDL_RenderCopy(gRenderer, textures[texture_index], NULL, &dest);
        }
    }

    //printf("Current cam coords: x: %d/ y: %d", cam_x, cam_y);
}

void render_map_from_file(SDL_Renderer* renderer, SDL_Texture* textures[36], char** current_map)
{
    //printf("Preparing to render\n");
    int start_col = cam_x / TILE_SIZE;
    int start_row = cam_y / TILE_SIZE;
    int end_col = (cam_x + WINDOW_WIDTH) / TILE_SIZE;
    int end_row = (cam_y + WINDOW_HEIGHT) / TILE_SIZE;

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

            char tile_type = current_map[row][col];
            int texture_index = tile_type - '0';

            if (texture_index < 0 || texture_index >= 36) {
                //printf("Invalid texture index: %d\n", texture_index);
                continue;
            }

            int tile_x = 0;
            int tile_y = 0;

            get_texture_dimensions(textures[texture_index], &tile_x, &tile_y);

            SDL_Rect dest = {
                (col * TILE_SIZE) - cam_x,
                (row * TILE_SIZE) - cam_y,
                tile_x, tile_y
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



