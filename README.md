allegro_framework
=================

A small framework written in C for use with the Allegro 5 library.

See `allegro_framework.h` or the example program for more detailed documentation of the framework.

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

### Resolution independence ###

If you design the game for a specific resolution, the framework will automatically take care of rescaling
graphics according to how you set it up. You can either rely on a second buffer bitmap by calling `setup_viewport()` where `use_buffer_bitmap = true` (best for bitmap heavy games), or a
transformation by calling `setup_viewport()` where `use_buffer_bitmap = false` (best for games relying a lot on drawing primitives). Call that function **after** `init_framework()`.

### Misc ###

* Waiting for keypress function.
* Basic random number generation (using `rand()`).
* Alt-tab behavior (whether the game should pause when it's not in focus).
* Alt-Enter toggles fullscreen mode.

### Mathlib ###

An extension to the framework. Useful constants, structs and functions for detecting collisions. Found in `mathlib.h` and `mathlib.c`.

Planned features
----------------

+ Simplifying input handling (generic).
+ Alternative game loop using fixed timestep.
