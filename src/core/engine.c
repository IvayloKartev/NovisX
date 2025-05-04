#include "engine.h"
#include "graphics.h"
#include "script.h"
#include <SDL2/SDL.h>
#include "../ui/menu.h"

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
            // checks for mouse clicks and keyboard presses types of events
            if (event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_KEYDOWN && event.key.keysym.sym != SDLK_ESCAPE) {

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

