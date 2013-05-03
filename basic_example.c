// gcc basic_example.c allegro_framework.c -o basic_example -lallegro-5.0.8-monolith-mt-debug -std=c99 -Wall

#include "allegro_framework.h"

void update()
{
    if (is_key_down(ALLEGRO_KEY_ESCAPE))
        quit();
}

void draw()
{
    
}

int main()
{
    init_framework("basic example", 640, 480, false);
    run_game_loop(update, draw);
    
    return 0;
}
