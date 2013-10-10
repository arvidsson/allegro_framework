#include "allegro_framework.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static ALLEGRO_EVENT_QUEUE *event_queue = NULL;
static ALLEGRO_DISPLAY *display = NULL;
static ALLEGRO_TIMER *timer = NULL;
static ALLEGRO_FILE *logfile = NULL;
static ALLEGRO_FONT *default_font = NULL;
static ALLEGRO_BITMAP *buffer = NULL;
static int scale_w, scale_h, scale_x, scale_y;
static bool is_using_buffer_bitmap = false;
static int viewport_width = 0, viewport_height = 0;

static bool is_done = false;
static bool is_paused = false;
static bool should_alt_tab_pause = true;

static bool keys[ALLEGRO_KEY_MAX] = { false };
static bool keys_pressed[ALLEGRO_KEY_MAX] = { false };
static bool keys_released[ALLEGRO_KEY_MAX] = { false };
static int mouse_x = 0, mouse_y = 0;
static int mouse_dx = 0, mouse_dy = 0;
static bool mouse_buttons[MAX_MOUSE_BUTTONS] = { false };
static bool mouse_buttons_pressed[MAX_MOUSE_BUTTONS] = { false };
static bool mouse_buttons_released[MAX_MOUSE_BUTTONS] = { false };

ALLEGRO_COLOR black_color;
ALLEGRO_COLOR white_color;

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

void init_framework(const char *window_title, int window_width, int window_height, bool fullscreen)
{
    logfile = al_fopen("log.txt", "w");
    
    if (!al_init())
        log_error("Failed to initialize allegro");
    
    if (!al_install_keyboard())
        log_error("Failed to install keyboard");
    
    if (!al_install_mouse())
        log_error("Failed to install mouse");
    
    if (!al_init_primitives_addon())
        log_error("Failed to init primitives addon");
    
    if (!al_init_image_addon())
        log_error("Failed to init image addon");
    
    al_init_font_addon();
    default_font = al_create_builtin_font();
    if (!default_font)
        log_error("Failed to create builtin font");
    
    event_queue = al_create_event_queue();
    if (!event_queue)
        log_error("Failed to create event queue");
    
    if (fullscreen)
        al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW);
    else
        al_set_new_display_flags(ALLEGRO_WINDOWED);
    
    //al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
    //al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);
    
    display = al_create_display(window_width, window_height);
    if (!display)
        log_error("Failed to create display @ %dx%d", window_width, window_height);
    al_set_window_title(display, window_title);
    viewport_width = get_window_width();
    viewport_height = get_window_height();
    
    timer = al_create_timer(1.0 / 60);
    if (!timer)
        log_error("Failed to create timer");
    
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_mouse_event_source());
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    
    srand(time(NULL));
    
    black_color = al_map_rgb(0, 0, 0);
    white_color = al_map_rgb(255, 255, 255);
    
    atexit(destroy_framework);
}

void destroy_framework()
{
    if (default_font) {
        al_destroy_font(default_font);
        default_font = NULL;
    }
    
    if (buffer) {
        al_destroy_bitmap(buffer);
        buffer = NULL;
    }
    
    if (timer) {
        al_destroy_timer(timer);
        timer = NULL;
    }
    
    if (display) {
        al_destroy_display(display);
        display = NULL;
    }
    
    if (event_queue) {
        al_destroy_event_queue(event_queue);
        event_queue = NULL;
    }
    
    if (logfile) {
        al_fclose(logfile);
        logfile = NULL;
    }
}

void setup_viewport(int width, int height, bool use_buffer_bitmap)
{
    is_using_buffer_bitmap = use_buffer_bitmap;
	
    if (use_buffer_bitmap) {
        buffer = al_create_bitmap(width, height);
        if (!buffer)
            log_error("Failed to create buffer bitmap @ %dx%d", width, height);
        
        int window_width = al_get_display_width(display);
        int window_height = al_get_display_height(display);
        
        // calculate scaling factor
        int sx = window_width / width;
        int sy = window_height / height;
        int scale = (sx < sy) ? sx : sy;
        
        // calculate how much the buffer should be scaled
        scale_w = width * scale;
        scale_h = height * scale;
        scale_x = (window_width - scale_w) / 2;
        scale_y = (window_height - scale_h) / 2;
    }
    else {
        int window_width = al_get_display_width(display);
        int window_height = al_get_display_height(display);
        
        // calculate scaling factor
        float sx = window_width / (float)width;
        float sy = window_height / (float)height;
         
        ALLEGRO_TRANSFORM trans;
        al_identity_transform(&trans);
        al_scale_transform(&trans, sx, sy);
        al_use_transform(&trans);
    }
    
    viewport_width = width;
    viewport_height = height;
}

void alt_tab_should_pause(bool yesno)
{
    should_alt_tab_pause = yesno;
}

void run_game_loop(void (*update_proc)(), void (*draw_proc)())
{
    bool should_redraw = true;
    al_start_timer(timer);
    
    while (!is_done) {
        ALLEGRO_EVENT event;
        al_wait_for_event(event_queue, &event);
        
        switch (event.type) {
            case ALLEGRO_EVENT_TIMER:
                should_redraw = true;
                if (!is_paused)
                    update_proc();
                memset(keys_pressed, false, sizeof(keys_pressed));
                memset(keys_released, false, sizeof(keys_pressed));
                memset(mouse_buttons_pressed, false, sizeof(mouse_buttons_pressed));
                memset(mouse_buttons_released, false, sizeof(mouse_buttons_released));
                mouse_dx = mouse_dy = 0;
                break;
            
            case ALLEGRO_EVENT_KEY_DOWN:
                keys[event.keyboard.keycode] = true;
                keys_pressed[event.keyboard.keycode] = true;
                break;
            
            case ALLEGRO_EVENT_KEY_UP:
                keys[event.keyboard.keycode] = false;
                keys_released[event.keyboard.keycode] = true;
                break;
            
            case ALLEGRO_EVENT_KEY_CHAR:
                if ((event.keyboard.modifiers & ALLEGRO_KEYMOD_ALT) &&
                    event.keyboard.keycode == ALLEGRO_KEY_ENTER) {
                    al_set_display_flag(display, ALLEGRO_FULLSCREEN_WINDOW, !(al_get_display_flags(display) & ALLEGRO_FULLSCREEN_WINDOW));
                    if (viewport_width != 0 && viewport_height != 0)
                        setup_viewport(viewport_width, viewport_height, is_using_buffer_bitmap);
                }
                break;
            
            case ALLEGRO_EVENT_MOUSE_AXES:
                mouse_x = event.mouse.x;
                mouse_y = event.mouse.y;
                mouse_dx = event.mouse.dx;
                mouse_dy = event.mouse.dy;
                break;
            
            case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
                mouse_buttons[event.mouse.button] = true;
                mouse_buttons_pressed[event.mouse.button] = true;
                break;
            
            case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
                mouse_buttons[event.mouse.button] = false;
                mouse_buttons_released[event.mouse.button] = true;
                break;
            
            case ALLEGRO_EVENT_DISPLAY_CLOSE:
                is_done = true;
                break;
            
            case ALLEGRO_EVENT_DISPLAY_SWITCH_OUT:
                if (should_alt_tab_pause)
                    is_paused = true;
                break;
            
            case ALLEGRO_EVENT_DISPLAY_SWITCH_IN:
                if (should_alt_tab_pause)
                    is_paused = false;
                break;
        }
        
        if (should_redraw && al_is_event_queue_empty(event_queue) && !is_paused) {
            should_redraw = false;
            if (buffer)
                al_set_target_bitmap(buffer);
            else
                al_set_target_bitmap(al_get_backbuffer(display));
            al_clear_to_color(al_map_rgb(0, 0, 0));
            
            draw_proc();
            
            if (buffer) {
                al_set_target_backbuffer(display);
                al_clear_to_color(al_map_rgb(0, 0, 0));
                al_draw_scaled_bitmap(buffer, 0, 0, al_get_bitmap_width(buffer), al_get_bitmap_height(buffer), scale_x, scale_y, scale_w, scale_h, 0);
            }
            
            al_flip_display();
        }
    }
}

void quit()
{
    is_done = true;
}

int get_window_width()
{
    return al_get_display_width(display);
}

int get_window_height()
{
    return al_get_display_height(display);
}

int get_viewport_width()
{
    return viewport_width;
}

int get_viewport_height()
{
    return viewport_height;
}

bool is_key_down(int keycode)
{
    return keys[keycode];
}

bool is_key_pressed(int keycode)
{
    return keys_pressed[keycode];
}

bool is_key_released(int keycode)
{
    return keys_released[keycode];
}

int get_mouse_x()
{
    return mouse_x;
}

int get_mouse_y()
{
    return mouse_y;
}

int get_mouse_dx()
{
    return mouse_dx;
}

int get_mouse_dy()
{
    return mouse_dy;
}

bool is_mouse_button_down(int mouse_button)
{
    return mouse_buttons[mouse_button];
}

bool is_mouse_button_pressed(int mouse_button)
{
    return mouse_buttons_pressed[mouse_button];
}

bool is_mouse_button_released(int mouse_button)
{
    return mouse_buttons_released[mouse_button];
}

int wait_for_keypress()
{
    ALLEGRO_EVENT event;
    do {
        al_wait_for_event(event_queue, &event);
    } while (event.type != ALLEGRO_EVENT_KEY_DOWN);
    return event.keyboard.keycode;
}

int get_random_int(int min, int max)
{
    return min + (rand() % (int)(max - min + 1));
}

float get_random_float(float min, float max)
{
    return min + ((float)rand() / ((float)RAND_MAX / (max - min)));
}

ALLEGRO_FONT* get_default_font()
{
    return default_font;
}
