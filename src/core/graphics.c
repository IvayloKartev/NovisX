#include "graphics.h"
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <SDL2/SDL_mixer.h>
static Mix_Music* gMusic = NULL;

static SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;
static TTF_Font* gFont = NULL;

bool graphics_init()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) return false;
    if (TTF_Init() == -1) return false;
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) return false;

    // creating a window and a renderer
    gWindow = SDL_CreateWindow("NovisX", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, 0);
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

void graphics_draw_text(const char* text, int x, int y)
{
    if (text == NULL || text[0] == '\0') {
        //printf("Error: Text is empty.\n");
        return;
    }

    if (!gFont) {
        printf("Error: Font not loaded: %s\n", TTF_GetError());
        return;
    }

    // using built-in methods to generate text surface
    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface* surface = TTF_RenderText_Blended(gFont, text, white);
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


SDL_Texture* create_transparent_rect_texture(int width, int height) {
    SDL_Color transparent_gray = {50, 50, 50, 100};
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
