// gcc example.c allegro_framework.c -o example -lallegro-5.0.8-monolith-mt-debug -std=c99 -Wall

/*
    TODO:
    
    init_options: struct contains everything that we need to setup - sent to init_framework
    - window_title
    - display_width
    - display_height
    - screen_width
    - screen_height
    - scaling_method: NO_SCALING, SCALE_TRANSFORM, SCALE_BUFFER
    - fullscreen
    - fps
    - callbacks
    - flags (KEYBOARD, MOUSE, etc..., RESIZABLE, )
    
    - resolution independence (both methods)
    - game loop
    - alt-tabbing?
    - INPUT HANDLING: Controllers
 */

#include "allegro_framework.h"
#include <stdio.h>

void logic()
{
    if (is_key_down(ALLEGRO_KEY_ESCAPE))
        quit();
}

void render()
{
    
}

int main()
{
    init_framework(640, 480, false, 60, logic, render);
    run_game_loop();
    
    return 0;
}
