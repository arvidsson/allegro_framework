// gcc basic_example.c allegro_framework.c -o basic_example -lallegro-5.0.8-monolith-mt-debug -std=c99 -Wall

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
    init_framework(640, 480, false);
    run_game_loop(logic, render);
    
    return 0;
}
