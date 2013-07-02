// example: a bouncing rectangle
#include "allegro_framework.h"

typedef struct {
    float x, y;
    float w, h;
    float dx, dy;
} rectangle;

rectangle r = { 50, 50, 50, 50, 3, 3 };

void update()
{
    if (is_key_down(ALLEGRO_KEY_ESCAPE)) {
        quit();
    }
    
    r.x += r.dx;
    r.y += r.dy;
    
    if (r.x < 0 || r.x + r.w > get_window_width()) {
        r.dx = -r.dx;
    }
    
    if (r.y < 0 || r.y + r.h > get_window_height()) {
        r.dy = -r.dy;
    }
}

void draw()
{
    al_draw_filled_rectangle(r.x, r.y, r.x + r.w, r.y + r.h, al_map_rgb(255, 0, 0));
}

int main()
{
    /* must be called first! */
    init_framework("example", 640, 480, false);
    
    /* optional, setup viewport to achieve resolution independence */
    setup_viewport(320, 240, false);
    
    /* optional, setup alt-tab behavior (true by default) */
    alt_tab_should_pause(false);
    
    /* the game loop runs until we call quit() */
    run_game_loop(update, draw);
    
    /* optional, called automatically when program ends */
    destroy_framework();
    
    return 0;
}
