#include "allegro_framework.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

ALLEGRO_EVENT_QUEUE *event_queue = NULL;
ALLEGRO_DISPLAY *display = NULL;
ALLEGRO_TIMER *timer = NULL;
ALLEGRO_FILE *logfile = NULL;
ALLEGRO_FONT *default_font = NULL;
ALLEGRO_BITMAP *buffer = NULL;
int scale_w, scale_h, scale_x, scale_y;

bool done = false;
bool keys[ALLEGRO_KEY_MAX] = { false };
bool keys_pressed[ALLEGRO_KEY_MAX] = { false };
bool keys_released[ALLEGRO_KEY_MAX] = { false };
int mouse_x = 0, mouse_y = 0;
bool mouse_moved = false;
int mouse_dx = 0, mouse_dy = 0;
bool mouse_buttons[MAX_MOUSE_BUTTONS] = { false };
bool mouse_buttons_pressed[MAX_MOUSE_BUTTONS] = { false };
bool mouse_buttons_released[MAX_MOUSE_BUTTONS] = { false };

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

void init_framework(const char *window_title, int display_width, int display_height, bool fullscreen)
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
    
    display = al_create_display(display_width, display_height);
    if (!display)
        log_error("Failed to create display @ %dx%d", display_width, display_height);
    al_set_window_title(display, window_title);
    
    timer = al_create_timer(1.0 / 60);
    if (!timer)
        log_error("Failed to create timer");
    
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_mouse_event_source());
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    
    black_color = al_map_rgb(0, 0, 0);
    white_color = al_map_rgb(255, 255, 255);
    
    srand(time(NULL));
    atexit(destroy_framework);
}

void destroy_framework()
{
    if (default_font) {
        al_destroy_font(default_font);
    }
    
    if (timer) {
        al_destroy_timer(timer);
    }
    
    if (display) {
        al_destroy_display(display);
    }
    
    if (event_queue) {
        al_destroy_event_queue(event_queue);
    }
    
    if (logfile) {
        al_fclose(logfile);
    }
}

void setup_buffer_bitmap(int width, int height)
{
    buffer = al_create_bitmap(width, height);
    if (!buffer) {
        log_error("Failed to create buffer bitmap @ %dx%d", width, height);
    }
        
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

void setup_transformation(int width, int height)
{
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

void run_game_loop(void (*update_proc)(), void (*draw_proc)())
{
    bool redraw = true;
    const double dt = 0.01;
    double current_time = al_get_time();
    double accumulator = 0.0;
    
    while (!done) {
        double new_time = al_get_time();
        double frame_time = new_time - current_time;
        current_time = new_time;
    
        ALLEGRO_EVENT event;
        al_wait_for_event(event_queue, &event);
        
        switch (event.type) {
            case ALLEGRO_EVENT_DISPLAY_CLOSE:
                done = true;
                break;
            
            case ALLEGRO_EVENT_TIMER:
                redraw = true;
                logic_proc();
                memset(keys_pressed, false, sizeof(keys_pressed));
                memset(keys_released, false, sizeof(keys_pressed));
                memset(mouse_buttons_pressed, false, sizeof(mouse_buttons_pressed));
                memset(mouse_buttons_released, false, sizeof(mouse_buttons_released));
                mouse_moved = false;
                break;
            
            case ALLEGRO_EVENT_KEY_DOWN:
                keys[event.keyboard.keycode] = true;
                keys_pressed[event.keyboard.keycode] = true;
                break;
            
            case ALLEGRO_EVENT_KEY_UP:
                keys[event.keyboard.keycode] = false;
                keys_released[event.keyboard.keycode] = true;
                break;
            
            case ALLEGRO_EVENT_MOUSE_AXES:
                mouse_x = event.mouse.x;
                mouse_y = event.mouse.y;
                mouse_moved = true;
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
        }
        
        if (redraw && al_is_event_queue_empty(event_queue)) {
            redraw = false;
            if (buffer) {
                al_set_target_bitmap(buffer);
            }
            al_clear_to_color(al_map_rgb(0, 0, 0));
            
            render_proc();
            
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
    done = true;
}

int get_window_width()
{
    return al_get_display_width(display);
}

int get_window_height()
{
    return al_get_display_height(display);
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

bool is_moused_moved()
{
    return mouse_moved;
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

float distance_between_points(Point p1, Point p2)
{
    return sqrt((p2.x - p1.x) * (p2.x - p1.x) + (p2.y - p1.y) * (p2.y - p1.y));
}

int line_segment_intercept(float ax1,float ay1,float ax2,float ay2,float bx1,float by1,float bx2,float by2){
    if(!same_side_of_line(ax1,ay1,ax2,ay2,bx1,by1,bx2,by2)&&!same_side_of_line(bx1,by1,bx2,by2,ax1,ay1,ax2,ay2)) return 1;
    return 0;                                                
}

bool points_are_same_side_of_line(Line l, Point p1, Point p2)
{
    return (((l.x1 - l.x2) * (p1.y - l.y2) - (l.y1 - l.y2) * (p1.x - l.x2)) * ((l.x1 - l.x2) * (p2.y - l.y2) - (l.y1 - l.y2) * (p2.x - l.x2)) >=0);
}

bool lines_intersect(Line l1, Line l2)
{
    return (!points_are_same_side_of_line(l1, {l2.x1, l2.y1}, {l2.x2, l2.y2}) && !points_are_same_side_of_line(l2, {l1.x1, l1.y1}, {l1.x2, l1.y2}));
}

bool rectangles_intersect(Rect r1, Rect r2)
{
    return !((r1.x + r1.w) < r2.x || (r1.y + r1.h) < r2.y || r1.x > (r2.x + r2.w) || r1.y > (r2.y + r2.h));
}

bool rectangle_contains_point(Rect r, Point p)
{
    return !(p.x < r.x || p.x > (r.x + r.w) || p.y < r.y || p.y > (r.y + r.h));
}

bool circles_intersect(Circle c1, Circle c2)
{
    float r = c1.r + c2.r;
    float dx = c1.x - c2.x;
    float dy = c1.y - c2.y;
    return (r * r) > (dx * dx + dy * dy);
}

bool circle_contains_point(Circle c, Point p)
{
    float dx = c.x - p.x;
    float dy = c.y - p.y;
    return ((dx * dx) + (dy * dy)) < (c.r * cr);
}

bool circle_and_rectangle_intersect(Circle c, Rect r)
{
    if ((r.x < c.x && c.x < (r.x + r.w)) && ((r.y - c.r) < c.y && cy < (r.y + r.h + c.r)))
        return true;
    
    if ((r.x - c.r < c.x && c.x < (r.x + r.w + c.r)) && (r.y < c.y && cy < (r.y + r.h)))
        return true;

    if (circle_contains_point(c, {r.x, r.y})) return true;
    if (circle_contains_point(c, {r.x + r.w, r.y})) return true;
    if (circle_contains_point(c, {r.x, r.y + r.h})) return true;
    if (circle_contains_point(c, {r.x + r.w, r.y + r.h})) return true;
    
    return false;
}
