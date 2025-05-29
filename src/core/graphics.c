#include "graphics.h"
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_image.h>

static Mix_Music* gMusic = NULL;

SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;
static TTF_Font* gFont = NULL;

int WINDOW_HEIGHT = 600;
int WINDOW_WIDTH = 800;

bool graphics_init()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) return false;
    if (TTF_Init() == -1) return false;
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) return false;

    // creating a window and a renderer
    gWindow = SDL_CreateWindow("NovisX", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);

    // initializing font
    gFont = TTF_OpenFont("assets/font.ttf", 24);
    if (!gFont) return false;

    // enable alpha blending
    SDL_SetRenderDrawBlendMode(gRenderer, SDL_BLENDMODE_BLEND);

    return true;
}

void graphics_play_music(const char* filename)
{
    if (gMusic) {
        Mix_HaltMusic();
        Mix_FreeMusic(gMusic);
    }
    gMusic = Mix_LoadMUS(filename);
    if (gMusic) {
        Mix_PlayMusic(gMusic, -1); // loop forever
    }
}

void graphics_shutdown()
{
    TTF_CloseFont(gFont);
    Mix_FreeMusic(gMusic);
    Mix_CloseAudio();
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    TTF_Quit();
    SDL_Quit();
}

void graphics_clear()
{
    SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
    SDL_RenderClear(gRenderer);
}

void graphics_present()
{
    SDL_RenderPresent(gRenderer);
}

SDL_Texture* graphics_load_texture(const char* filename)
{
    SDL_Surface* surface = SDL_LoadBMP(filename); // bmps are easier to load
    if (!surface) {
        printf("Failed to load image %s: %s\n", filename, SDL_GetError());
        return NULL;
    }
    // to create texture, a surface (BMP) and a renderer is needed
    SDL_Texture* texture = SDL_CreateTextureFromSurface(gRenderer, surface);
    // surface is immediately removed to save memory
    SDL_FreeSurface(surface);
    return texture;
}

void graphics_draw_texture(SDL_Texture* texture, int x, int y)
{
    if (!texture)
    {
        printf("Warning: Tried to draw NULL texture!\n");
        return;
    }
    SDL_Rect dst = {x, y, 0, 0};
    // uses generated texture and uses coordinates to calculate width and height
    SDL_QueryTexture(texture, NULL, NULL, &dst.w, &dst.h);
    // in order for a texture to appear, a copy of it has to be rendered
    SDL_RenderCopy(gRenderer, texture, NULL, &dst);
}

void graphics_scale_texture(SDL_Texture* texture, int x, int y, int scale)
{
    if (!texture)
    {
        printf("Warning: Tried to draw NULL texture!\n");
        return;
    }

    int original_w, original_h;
    // gets the dimensions of a texture from the image
    SDL_QueryTexture(texture, NULL, NULL, &original_w, &original_h);

    // scaled dimensions
    int scaled_w = original_w * scale / 100;
    int scaled_h = original_h * scale / 100;
    SDL_Rect dst = {x, y, scaled_w, scaled_h};
    // Render the texture with the scaled dimensions
    SDL_RenderCopy(gRenderer, texture, NULL, &dst);
}


void graphics_draw_sized_texture(SDL_Texture* texture, int x, int y, int width, int height)
{
    if (!texture)
    {
        printf("Warning: Tried to draw NULL texture!\n");
        return;
    }

    SDL_Rect dst = {x, y, 0, 0};
    int original_w, original_h;

    // Get the original dimensions of the texture
    SDL_QueryTexture(texture, NULL, NULL, &original_w, &original_h);

    // Use specified dimensions if provided, otherwise use the texture's original size
    dst.w = (width > 0) ? width : original_w;
    dst.h = (height > 0) ? height : original_h;

    // Define the source rectangle to crop if necessary
    SDL_Rect src = {0, 0, original_w, original_h};

    // Adjust source rectangle if the desired dimensions are smaller than the original
    if (width > 0 && width < original_w) {
        src.w = width;
    }
    if (height > 0 && height < original_h) {
        src.h = height;
    }

    // Render the texture with the specified or default dimensions
    SDL_RenderCopy(gRenderer, texture, &src, &dst);
}

void graphics_draw_text(const char* text, int x, int y, char* font_path, int font_size, SDL_Texture* text_bg, int alpha, int r, int g, int b)
{
    if (text == NULL || text[0] == '\0') {
        //printf("Error: Text is empty.\n");
        return;
    }

    SDL_Color color = {r, g, b, alpha};

    if (!gFont) {
        printf("Error: Font not loaded: %s\n", TTF_GetError());
        return;
    }

    TTF_Font* font = NULL;
    if(font_path && font_size > 0) font = TTF_OpenFont(font_path, font_size);
    // using built-in methods to generate text surface

    SDL_Surface* surface = NULL;
    if(!font) surface = TTF_RenderText_Blended(gFont, text, color);
    else surface = TTF_RenderText_Blended(font, text, color);

    TTF_CloseFont(font);
    if (!surface) {
        printf("Error creating text surface: %s\n", TTF_GetError());
        return;
    }

    // texture requires surface and renderer
    SDL_Texture* texture = SDL_CreateTextureFromSurface(gRenderer, surface);
    if (!texture) {
        printf("Error creating texture from surface: %s\n", SDL_GetError());
        SDL_FreeSurface(surface);
        return;
    }

    // a copy is rendered in order to appear on the screen
    SDL_Rect dst = {x, y, surface->w, surface->h};

    if(text_bg)
    {
            //printf("About to draw text bg\n");
            SDL_Rect dst = {x - 100, y, 0, 0};
            // uses generated texture and uses coordinates to calculate width and height
            SDL_QueryTexture(text_bg, NULL, NULL, &dst.w, &dst.h);
            // in order for a texture to appear, a copy of it has to be rendered
            SDL_RenderCopy(gRenderer, text_bg, NULL, &dst);
    }

    SDL_RenderCopy(gRenderer, texture, NULL, &dst);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void graphics_fade_in(SDL_Texture* texture, int alpha) {

    if (alpha > 255) alpha = 255;

    SDL_SetTextureAlphaMod(texture, alpha);

    //graphics_clear();
    graphics_draw_texture(texture, 0, 0);
    //graphics_present();
}

SDL_Texture* combine_textures(SDL_Texture** textures, int* x_positions, int* y_positions, int* x_widths, int* y_heights, int number_of_buttons)
{
    if (number_of_buttons <= 0) {
        SDL_Log("combine_textures: No buttons to render.");
        return NULL;
    }


    // Determine total size needed
    int menu_width = 0;
    int menu_height = 0;
    for (int i = 0; i < number_of_buttons; i++) {
        int right = x_positions[i] + x_widths[i];
        int bottom = y_positions[i] + y_heights[i];
        if (right > menu_width) menu_width = right;
        if (bottom > menu_height) menu_height = bottom;
    }

    // Check for maximum texture size
    if (menu_width > 16384 || menu_height > 16384) {
        SDL_Log("combine_textures: Menu size exceeds maximum texture dimensions (%d x %d)", menu_width, menu_height);
        return NULL;
    }

    // Create target texture
    SDL_Texture* menu = SDL_CreateTexture(
        gRenderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET,
        menu_width,
        menu_height
    );

    if (menu == NULL) {
        SDL_Log("SDL_CreateTexture failed: %s", SDL_GetError());
        return NULL;
    }

    // Set target to the new texture and clear with transparency
    SDL_SetRenderTarget(gRenderer, menu);
    SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0); // transparent
    SDL_RenderClear(gRenderer);

    for (int i = 0; i < number_of_buttons; i++) {
        SDL_Rect dst_rect = {
            x_positions[i],
            y_positions[i],
            x_widths[i],
            y_heights[i]
        };
        if (SDL_RenderCopy(gRenderer, textures[i], NULL, &dst_rect) != 0) {
            SDL_Log("SDL_RenderCopy failed for button %d: %s", i, SDL_GetError());
        }
    }

    // Reset the render target to default
    SDL_SetRenderTarget(gRenderer, NULL);

    SDL_SetTextureBlendMode(menu, SDL_BLENDMODE_BLEND);

    return menu;
}


SDL_Texture* create_transparent_rect_texture(int width, int height, int alpha, int r, int g, int b) {

    SDL_Color transparent_gray = {r, g, b, alpha};

    // creating an RGBA texture with render target access
    SDL_Texture* texture = SDL_CreateTexture(
        gRenderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET,
        width,
        height
    );

    if (!texture) {
        SDL_Log("Failed to create transparent texture: %s", SDL_GetError());
        return NULL;
    }

    // enabling blending for transparency
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

    // saving current render target (usually main screen)
    SDL_Texture* prev_target = SDL_GetRenderTarget(gRenderer);

    SDL_SetRenderTarget(gRenderer, texture);
    SDL_SetRenderDrawColor(gRenderer, transparent_gray.r, transparent_gray.g, transparent_gray.b, transparent_gray.a);
    SDL_RenderClear(gRenderer);

    // restoring previous render target
    SDL_SetRenderTarget(gRenderer, prev_target);

    return texture;
}

SDL_Texture* graphics_load_text(char* text)
{
    // using built-in methods to generate text surface
    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface* surface = TTF_RenderText_Blended(gFont, text, white);
    if (!surface) {
        printf("Error creating text surface: %s\n", TTF_GetError());
    }

    // texture requires surface and renderer
    SDL_Texture* texture = SDL_CreateTextureFromSurface(gRenderer, surface);
    if (!texture) {
        printf("Error creating texture from surface: %s\n", SDL_GetError());
        SDL_FreeSurface(surface);
    }

    return texture;
}

SDL_Texture* graphics_resize_texture(SDL_Texture* texture, int new_width, int new_height)
{
    if (!texture)
    {
        printf("Warning: Tried to resize a NULL texture!\n");
        return NULL;
    }

    int original_w, original_h;
    // original dimensions of the texture
    SDL_QueryTexture(texture, NULL, NULL, &original_w, &original_h);

    // new texture with the specified dimensions
    SDL_Texture* resized_texture = SDL_CreateTexture(
        gRenderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET,
        new_width, new_height
    );

    if (!resized_texture)
    {
        printf("Error: Unable to create resized texture: %s\n", SDL_GetError());
        return NULL;
    }

    // Set the new texture as the rendering target
    SDL_SetRenderTarget(gRenderer, resized_texture);

    // Clear the texture (optional, but ensures transparency)
    SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0);
    SDL_RenderClear(gRenderer);

    SDL_Rect dst = {0, 0, new_width, new_height};

    // Copy the original texture onto the new one with resizing
    SDL_RenderCopy(gRenderer, texture, NULL, &dst);

    // Reset the rendering target to the default (screen)
    SDL_SetRenderTarget(gRenderer, NULL);

    return resized_texture;
}

SDL_Texture* graphics_text_to_texture(const char* text, int x, int y, char* font_path, int font_size, SDL_Texture* text_bg, int alpha, int r, int g, int b)
{
    if (text == NULL || text[0] == '\0') {
        //printf("Error: Text is empty.\n");
        return NULL;
    }

    SDL_Color color = {r, g, b, alpha};

    if (!gFont) {
        printf("Error: Font not loaded: %s\n", TTF_GetError());
        return NULL;
    }

    TTF_Font* font = NULL;
    if(font_path && font_size > 0) font = TTF_OpenFont(font_path, font_size);
    // using built-in methods to generate text surface

    SDL_Surface* surface = NULL;
    if(!font) surface = TTF_RenderText_Blended(gFont, text, color);
    else surface = TTF_RenderText_Blended(font, text, color);

    SDL_Texture* texture = SDL_CreateTextureFromSurface(gRenderer, surface);
    // surface is immediately removed to save memory
    SDL_FreeSurface(surface);
    return texture;
}

void get_texture_dimensions(SDL_Texture* texture, int* width, int* height)
{
    if (!texture) {
        //printf("Error: NULL texture passed to get_texture_dimensions\n");
        if (width) *width = 0;
        if (height) *height = 0;
        return;
    }

    if (SDL_QueryTexture(texture, NULL, NULL, width, height) != 0) {
        //printf("Error querying texture dimensions: %s\n", SDL_GetError());
        if (width) *width = 0;
        if (height) *height = 0;
    }
}

SDL_Texture* graphics_load_texture_png(const char* filename)
{
    SDL_Surface* surface = IMG_Load(filename);
    if (!surface) {
        printf("Failed to load image %s: %s\n", filename, IMG_GetError());
        return NULL;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(gRenderer, surface);
    SDL_FreeSurface(surface);

    if (!texture) {
        printf("Failed to create texture from %s: %s\n", filename, SDL_GetError());
        return NULL;
    }

    return texture;
}
