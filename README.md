allegro_framework
=================

A small framework written in C for use with the Allegro 5 library.

See `allegro_framework.h` for more detailed documentation of the framework,
and the example below for a quick intro on how to use it.

Features
--------

### Error handling ###

Use `log_error()` to log an error message which will forcefully quit the program. `destroy_framework()` is
called automatically and will clean up everything that the framework has put in place. Use `atexit()` if you want to
automatically clean up your own stuff when errors happen.

### Logging ###

Simple logfile functionality exists. It was mostly implemented because the framework somehow needs to be able to
report when errors happen. Use `log_message()` for simple debug messages, otherwise `log_warning()` for non-fatal errors and `log_error()`
for fatal errors.

### Initializes Allegro ###

`init_framework()` will setup allegro, useful addons, and create the basic things we need, such as an event queue
and a display.

### Game loop ###

`run_game_loop()` starts the game loop. This function will call the two callbacks that you provide to the function until you
ask the game to quit.

### Simplified input ###

The input from keyboard and mouse is polled each frame. Functions, such as `is_key_down()`, should be used in the update method defined
by the user in order to check for the input.

### Random number generation ###

* Uses `rand()`, and `srand()` is called in `init_framework()`.
* Random number generation between a range (e.g. `get_random_int()`).
* `one_in()` and `roll_dice()`

### Misc ###

* Waiting for keypress function.
* Alt-tab behavior (whether the game should pause when it's not in focus).
* Alt-enter toggles fullscreen mode.

### Mathlib ###

An extension to the framework. Useful constants, structs and functions for detecting collisions. Found in `mathlib.h` and `mathlib.c`.

example
-------

```c++
// example: a bouncing rectangle
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
