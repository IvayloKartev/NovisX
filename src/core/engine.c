#include "engine.h"
#include "graphics.h"
#include "parser.h"
#include <SDL.h>
#include "../ui/menu.h"
#include "../entity/player.h"
#include "../ui/map.h"
#include "../screen/page.h"
#include "../ui/components/components.h"
#include "parser.h"

bool awaiting_choice = false;
bool menu_visible = false;
bool page_show = false;
static bool running = true;
int player_speed = 10;

bool engine_init()
{
    if (!graphics_init()) return false;
    if (!script_load_xml("assets_platformer/start.xml")) return false;
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
                    //printf("MOVING UP\n");
                    if (can_jump)
                    {
                        printf("JUMP\n");
                        player_jump();
                    }
                    else
                        move_player("up", player_speed);
                }
                else if (event.key.keysym.sym == SDLK_DOWN)
                {
                    printf("MOVING DOWN\n");
                    move_player("down", player_speed);
                }
                else if (event.key.keysym.sym == SDLK_RIGHT)
                {
                    printf("MOVING RIGHT\n");
                    move_player("right", player_speed);
                }
                else if (event.key.keysym.sym == SDLK_LEFT)
                {
                    printf("MOVING LEFT\n");
                    move_player("left", player_speed);
                }
                else if (event.key.keysym.sym == SDLK_SPACE)
                {
                    player_jump();
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
                    if(page_show) handle_page_event(x, y);
                }
                script_next();
            }

            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
                menu_visible = !menu_visible;
            }
            if (event.type == SDL_WINDOWEVENT)
            {
                printf("Window event: %d\n", event.window.event);
            }
        }

        script_update();
        graphics_clear();
        script_render();
        if(page_show)
        {
            render_page();
        }
        if(show_player) update_player();
        if(show_player) render_player();
        if(menu_visible) render_menu();
        graphics_present();

        SDL_Delay(16); // roughly 60 fps
    }
}

void engine_shutdown() {
    script_unload();
    graphics_shutdown();
}

