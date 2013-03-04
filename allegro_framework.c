#include "allegro_framework.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ALLEGRO_EVENT_QUEUE *event_queue = NULL;
ALLEGRO_DISPLAY *display = NULL;
ALLEGRO_TIMER *timer = NULL;
ALLEGRO_FILE *logfile = NULL;
bool done = false;
bool keys[ALLEGRO_KEY_MAX] = { false };

// callbacks
typedef void (*function_ptr)();
function_ptr logic_proc;
function_ptr render_proc;

void write_logfile(int log_level, const char *format, ...)
{
    char buffer[4096];
    
    if (log_level == LOG_WARNING)
        al_fputs(logfile, "WARNING: ");
    else if (log_level == LOG_ERROR)
        al_fputs(logfile, "ERROR: ");
    
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
    al_fputs(logfile, buffer);
    al_fputs(logfile, "\n");
    
    if (log_level == LOG_ERROR)
        exit(1);
}

void init_framework(int display_width, int display_height, bool fullscreen, int fps, void (*logic_callback)(), void (*render_callback)())
{
    logfile = al_fopen("log.txt", "w");
    
    if (!al_init())
        log_error("Failed to initialize allegro");
    
    if (!al_install_keyboard())
        log_error("Failed to install keyboard");
    
    event_queue = al_create_event_queue();
    if (!event_queue)
        log_error("Failed to create event queue");
    
    if (fullscreen)
        al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW);
    else
        al_set_new_display_flags(ALLEGRO_WINDOWED);
    
    display = al_create_display(display_width, display_height);
    if (!display)
        log_error("Failed to create display @ %dx%d", display_width, display_height);
    
    timer = al_create_timer(1.0 / fps);
    if (!timer)
        log_error("Failed to create timer @ %d fps", fps);
    
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    
    // setup callbacks
    logic_proc = logic_callback;
    render_proc = render_callback;
    
    atexit(destroy_framework);
}

void destroy_framework()
{
    if (timer)
        al_destroy_timer(timer);
    if (display)
        al_destroy_display(display);
    if (event_queue)
        al_destroy_event_queue(event_queue);
    if (logfile)
        al_fclose(logfile);
}

void game_loop()
{
    bool redraw = true;
    al_start_timer(timer);
    
    while (!done) {
        ALLEGRO_EVENT event;
        al_wait_for_event(event_queue, &event);

        if (event.type == ALLEGRO_EVENT_TIMER) {
            redraw = true;
            logic_proc();
        }
        else if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
            if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
                quit();
        }
        
        if (redraw && al_is_event_queue_empty(event_queue)) {
            redraw = false;
            al_clear_to_color(al_map_rgb(0, 0, 0));
            render_proc();
            al_flip_display();
        }
    }
}

void quit()
{
    done = true;
}

int wait_for_keypress()
{
    ALLEGRO_EVENT event;
    do
        al_wait_for_event(event_queue, &event);
    while (event.type != ALLEGRO_EVENT_KEY_DOWN);
    return event.keyboard.keycode;
}
