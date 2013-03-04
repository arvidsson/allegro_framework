// gcc example.c allegro_framework.c -o example -lallegro-5.0.8-monolith-mt-debug -std=c99 -Wall

/*
    TODO:
    - init/destroy allegro, create display & stuff required
    - resolution independence (both methods)
    - game loop
    - timer & fixed timestep?
    - alt-tabbing?
    - simple input handling
    - INPUT HANLDING: Controllers, and other functions so that
    we can simplify the game loop
 */

#include "allegro_framework.h"
#include <stdio.h>

void logic()
{
    printf("update_logic called\n");
}

void render()
{
    printf("draw_graphics called\n");
}

int main()
{
    init_framework(640, 480, false, 60, logic, render);
    game_loop();
    
    return 0;
}
