#ifndef ALLEGRO_FRAMEWORK_H
#define ALLEGRO_FRAMEWORK_H

#ifdef __cplusplus
   extern "C" {
#endif

#include <allegro5/allegro.h>

/*
    Logging macros. Usage:
    - log_message: for debug messages
    - log_warning: for non-fatal errors, i.e. continue program execution
    - log_error:   for fatal errors, program will be forcefully terminated
 */
#define log_message(...) write_logfile(LOG_MESSAGE, __VA_ARGS__);
#define log_warning(...) write_logfile(LOG_WARNING, __VA_ARGS__);
#define log_error(...)   write_logfile(LOG_ERROR, __VA_ARGS__);

// logging levels (internal use)
enum {
    LOG_MESSAGE,
    LOG_WARNING,
    LOG_ERROR
};

/*
    Writes a message to the logfile. Use the macros above instead of using this
    function directly. 
 */
void write_logfile(int log_level, const char *format, ...);

/*
    Initializes the framework. This must be called before you use anything else
    in this framework!
 */
void init_framework(int display_width, int display_height, bool fullscreen, void (*logic_callback)(), void (*render_callback)());

/*
    Destroys everything we need to clean up when it is time to quit the program.
    This function is called automatically at program exit.
 */
void destroy_framework();

/*
    Runs the game loop; the heart of the game!
  */
void run_game_loop();

/*
    Will make the game loop stop running.
 */
void quit();

/*
    Returns true if a key on the keyboard is held down.
  */
bool is_key_down(int keycode);

/*
    Waits until a key is pressed on the keyboard. It returns the keycode of the
    key that was pressed.
 */
int wait_for_keypress();

/*
    Returns a random number between (max - 1) and min.
  */
int get_random_int(int max, int min);

#ifdef __cplusplus
   }
#endif

#endif
