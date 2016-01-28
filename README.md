allegro_framework
===========

Tiny Allegro 5 C framework.

> Quickly prototype ideas or get acquainted with Allegro 5 without having to reinvent the game loop!

Features
--------

* easy setup of allegro and addons
* game loop
* simplified input
* error handling and logging
* random number generation
* basic collision detection

Install
------------

Include ```allegro_framework.c``` and ```allegro_framework.h``` in your project.

Example
-------

```c
#include "allegro_framework.h"

Rectangle r = { 50, 50, 50, 50 };
Velocity v = { 3, 3 };

void update()
{
    if (is_key_down(ALLEGRO_KEY_ESCAPE)) {
        quit();
    }

    r.x += v.dx;
    r.y += v.dy;

    if (r.x < 0 || r.x + r.w > get_window_width()) {
        v.dx = -v.dx;
    }

    if (r.y < 0 || r.y + r.h > get_window_height()) {
        v.dy = -v.dy;
    }
}

void render()
{
    al_draw_filled_rectangle(r.x, r.y, r.x + r.w, r.y + r.h, red_color);
}

int main(int argc, char *argv[])
{
    // must be called first!
    init_framework("example", 640, 480, false);

    // the game loop runs until we call quit()
    run_game_loop(update, render);

    return 0;
}
```

License
-------
MIT (c) arvidsson
