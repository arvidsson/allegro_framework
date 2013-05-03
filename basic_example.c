// gcc basic_example.c allegro_framework.c -o basic_example -lallegro-5.0.8-monolith-mt-debug -std=c99 -Wall

#include "allegro_framework.h"

Rect r = { 100, 100, 100, 100};
float dx = 5, dy = 5;

void update()
{
    if (is_key_down(ALLEGRO_KEY_ESCAPE)) {
        quit();
    }
    
    r.x += dx;
    r.y += dy;
    
    if (r.x < 0 || r.x + r.w > get_window_width()) {
        dx = -dx;
    }
    if (r.y < 0 || r.y + r.h > get_window_height()) {
        dy = -dy;
    }
}

void draw()
{
    al_draw_filled_rectangle(r.x, r.y, r.x + r.w, r.y + r.h, al_map_rgb(255, 0, 0));
}

int main()
{
    init_framework("basic example", 640, 480, false);
    run_game_loop(update, draw);
    
    return 0;
}
