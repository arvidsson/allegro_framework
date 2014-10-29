#ifndef INCLUDED_ALLEGRO_FRAMEWORK_H
#define INCLUDED_ALLEGRO_FRAMEWORK_H

#ifdef __cplusplus
   extern "C" {
#endif

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>

/*
    Logging macros. Usage:
    - log_message: for debug messages
    - log_warning: for non-fatal errors, i.e. continue program execution
    - log_error:   for fatal errors, program will be forcefully terminated
 */
#define log_message(...) write_logfile(LOG_MESSAGE, __VA_ARGS__);
#define log_warning(...) write_logfile(LOG_WARNING, __VA_ARGS__);
#define log_error(...)   write_logfile(LOG_ERROR, __VA_ARGS__);

// logging levels (used in write_logfile)
enum {
    LOG_MESSAGE,
    LOG_WARNING,
    LOG_ERROR
};

/*
    Writes a message to the logfile.
    Use the macros above instead of using this function directly.
 */
void write_logfile(int log_level, const char *format, ...);

/*
    Initializes the framework.
    This must be called before you use anything else in this framework!
 */
void init_framework(const char *title, int window_width, int window_height, bool fullscreen);

/*
    Destroys everything we need to clean up when it is time to quit the program.
    This function is called automatically at program exit.
 */
void destroy_framework();

/*
    Sets alt-tab behavior (switching out & in from an application).
    If enabled, the game logic and rendering will pause until the game
    becomes active again. This is enabled by default.
 */
void alt_tab_should_pause(bool true_or_false);

/*
    Runs the game loop; the heart of the game!

    update_proc() and draw_proc() are function pointers you need to define yourself.
    Will call update_proc() 60 times per second.
    Will call render_proc() 60 times a second if there is no other events to deal with.
    If there is nothing else to do, the game loop will sleep.
 */
void run_game_loop(void (*update_proc)(), void (*render_proc)());

/*
    Stops the game loop.
 */
void quit();

/*
    Returns the width of the window.
 */
int get_window_width();

/*
    Returns the height of the window.
 */
int get_window_height();

/*
    Returns true if a key on the keyboard is held down.
 */
bool is_key_down(int keycode);

/*
    Returns true if a key on the keyboard was pressed.
 */
bool is_key_pressed(int keycode);

/*
    Returns true if a key on the keyboard was released.
 */
bool is_key_released(int keycode);

// helper mouse input enum
enum {
    MOUSE_LEFT_BUTTON,
    MOUSE_RIGHT_BUTTON,
    MOUSE_MIDDLE_BUTTON,
    MAX_MOUSE_BUTTONS
};

/*
    Returns mouse x coordinate.
 */
int get_mouse_x();

/*
    Returns mouse y coordinate.
 */
int get_mouse_y();

/*
    Returns mouse delta movement in x since last frame.
 */
int get_mouse_dx();

/*
    Returns mouse delta movement in y since last frame.
 */
int get_mouse_dy();

/*
    Returns true if a mouse button is held down.
 */
bool is_mouse_button_down(int mouse_button);

/*
    Returns true if a mouse button was pressed.
 */
bool is_mouse_button_pressed(int mouse_button);

/*
    Returns true if a mouse button was released.
 */
bool is_mouse_button_released(int mouse_button);

/*
    Waits until a key is pressed on the keyboard.
    Returns the keycode of the key that was pressed.
 */
int wait_for_keypress();

/*
    Returns a random integer between [min, max].
 */
int get_random_int(int min, int max);

/*
    Returns a random float between [min, max].
 */
float get_random_float(float min, float max);

/*
    Returns true if the random number is one in x.
 */
bool one_in(int chance);

/*
    Returns the result of rolling dice with a number of sides.
*/
int roll_dice(int number, int sides);

/*
    Returns a default font. Mainly used for debugging purposes.
 */
ALLEGRO_FONT* get_default_font();

/*
    Default colors.
 */
extern ALLEGRO_COLOR black_color;
extern ALLEGRO_COLOR white_color;
extern ALLEGRO_COLOR dark_grey_color;
extern ALLEGRO_COLOR grey_color;
extern ALLEGRO_COLOR light_grey_color;
extern ALLEGRO_COLOR red_color;
extern ALLEGRO_COLOR green_color;
extern ALLEGRO_COLOR dark_green_color;
extern ALLEGRO_COLOR blue_color;
extern ALLEGRO_COLOR yellow_color;
extern ALLEGRO_COLOR cyan_color;
extern ALLEGRO_COLOR magenta_color;
extern ALLEGRO_COLOR maroon_color;
extern ALLEGRO_COLOR purple_color;
extern ALLEGRO_COLOR lime_color;
extern ALLEGRO_COLOR olive_color;
extern ALLEGRO_COLOR navy_color;
extern ALLEGRO_COLOR teal_color;
extern ALLEGRO_COLOR brown_color;

#ifdef __cplusplus
   }
#endif

#endif
