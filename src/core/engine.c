#include "engine.h"
#include "graphics.h"
#include "script.h"
#include <SDL.h>
#include "menu.h"
#include "player.h"
#include "map.h"

extern bool awaiting_choice;
extern bool menu_visible = false;
static bool running = true;

bool engine_init()
{
    if (!graphics_init()) return false;
    if (!script_load("assets/script.txt")) return false;
    return true;
}

void engine_run()
{
    while (running)
    {
        SDL_Event event;
        // the event loop
        while (SDL_PollEvent(&event))
        {

            // when quit button is pressed - stops the while loop
            if (event.type == SDL_QUIT)
            {
                running = false;
            }

            if(menu_event_handler(&event)) continue;

            if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_UP)
                {
                    printf("MOVING UP\n");
                    move_player("up", 10);
                }
                else if (event.key.keysym.sym == SDLK_DOWN)
                {
                    printf("MOVING DOWN\n");
                    move_player("down", 10);
                }
                else if (event.key.keysym.sym == SDLK_RIGHT)
                {
                    printf("MOVING RIGHT\n");
                    move_player("right", 10);
                }
                else if (event.key.keysym.sym == SDLK_LEFT)
                {
                    printf("MOVING LEFT\n");
                    move_player("left", 10);
                }
            }
            // checks for mouse clicks and keyboard presses types of events
            if (event.type == SDL_MOUSEBUTTONDOWN || (event.type == SDL_KEYDOWN && event.key.keysym.sym != SDLK_ESCAPE && event.key.keysym.sym != SDLK_UP && event.key.keysym.sym != SDLK_DOWN && event.key.keysym.sym != SDLK_RIGHT && event.key.keysym.sym != SDLK_LEFT)) {

                // checks if particularly mouse is clicked
                if (event.type == SDL_MOUSEBUTTONDOWN) {
                    int x = event.button.x;
                    int y = event.button.y;
                    // calling script_choice_click with different index depending on the option
                    if (awaiting_choice) {
                        if (y > 450 && y < 480) script_choice_click(0);
                        else if (y > 500 && y < 530) script_choice_click(1);
                    }
                }
                script_next();
            }

            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
                menu_visible = !menu_visible;
            }
        }

        script_update();
        graphics_clear();
        script_render();
        if(menu_visible) render_menu();
        graphics_present();

        SDL_Delay(16); // roughly 60 fps
    }
}

void engine_shutdown() {
    script_unload();
    graphics_shutdown();
}

