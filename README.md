allegro_framework
========

A simple framework written in C for use with the Allegro 5 library.

See allegro_framework.h for more proper documentation of the framework. The example
program should show in proper detail how to use the framework.

Features:

- Error handling: use log_error() to log an error message which will forcefully quit the program. destroy_framework() is
called automatically and will clean up everything that the framework has put in place. Use atexit() (stdlib.h) if you want to
automatically clean up your own stuff when errors happen.

- Logging: simple logfile functionality exists, it was mostly implemented because the framework somehow needs to be able to
report when errors happen. Use log_message for simple debug messages, otherwise log_warning for non-fatal errors and log_error
for fatal errors.

- Initializes core allegro features: init_framework will setup allegro and create the basic things we need, such as an event queue
and a display.

- Game loop: EXPLAIN!

- Alt-tabbing: the framework automatically takes care of what should happen when the user alt-tabs. You can disable this entirely
if you want by calling disable_alt_tabbing().

- Resolution independence: if you design the game for a specific resolution, the framework will automatically take care of rescaling
graphics according to how you set it up. You can either rely on a second buffer bitmap (best for bitmap heavy games), or a
transformation (best for games relying a lot on drawing primitives).

- Simplifying input: handling input is simplified, as you can register id's which are then tied to whatever kind of input (keyboard,
mouse, joystick, etc) which makes it easier to load input configurations. See input_example.c for a better explanation :)

- Misc utility functions: waiting for a keypress,