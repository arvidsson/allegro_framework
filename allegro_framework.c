#include "allegro_framework.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <assert.h>

static ALLEGRO_EVENT_QUEUE *event_queue = NULL;
static ALLEGRO_DISPLAY *display = NULL;
static ALLEGRO_TIMER *timer = NULL;
static ALLEGRO_FILE *logfile = NULL;
static ALLEGRO_FONT *default_font = NULL;

static bool is_done = false;
static bool is_paused = false;
static bool should_alt_tab_pause = true;

static bool keys[ALLEGRO_KEY_MAX] = { false };
static bool keys_pressed[ALLEGRO_KEY_MAX] = { false };
static bool keys_released[ALLEGRO_KEY_MAX] = { false };

static int mouse_x = 0, mouse_y = 0;
static int mouse_old_x = 0, mouse_old_y = 0;
static bool mouse_buttons[MAX_MOUSE_BUTTONS] = { false };
static bool mouse_buttons_pressed[MAX_MOUSE_BUTTONS] = { false };
static bool mouse_buttons_released[MAX_MOUSE_BUTTONS] = { false };

ALLEGRO_COLOR black_color;
ALLEGRO_COLOR white_color;
ALLEGRO_COLOR dark_grey_color;
ALLEGRO_COLOR grey_color;
ALLEGRO_COLOR light_grey_color;
ALLEGRO_COLOR red_color;
ALLEGRO_COLOR green_color;
ALLEGRO_COLOR dark_green_color;
ALLEGRO_COLOR blue_color;
ALLEGRO_COLOR yellow_color;
ALLEGRO_COLOR cyan_color;
ALLEGRO_COLOR magenta_color;
ALLEGRO_COLOR maroon_color;
ALLEGRO_COLOR purple_color;
ALLEGRO_COLOR lime_color;
ALLEGRO_COLOR olive_color;
ALLEGRO_COLOR navy_color;
ALLEGRO_COLOR teal_color;
ALLEGRO_COLOR brown_color;

void write_logfile(int log_level, const char *format, ...)
{
    char buffer[4096];

    if (!logfile) {
        logfile = al_fopen("log.txt", "w");
    }

    if (log_level == LOG_WARNING) {
        al_fputs(logfile, "WARNING: ");
    }
    else if (log_level == LOG_ERROR) {
        al_fputs(logfile, "ERROR: ");
    }

    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    al_fputs(logfile, buffer);
    al_fputs(logfile, "\n");

    if (log_level == LOG_ERROR) {
        exit(1);
    }
}

void init_framework(const char *title, int window_width, int window_height, bool fullscreen)
{
    if (!al_init()) {
        log_error("Failed to initialize allegro");
    }

	atexit(destroy_framework);

	al_set_exe_name(title);
	al_set_app_name(title);

	// set correct resource directory
	ALLEGRO_PATH *path = al_get_standard_path(ALLEGRO_RESOURCES_PATH);
	al_change_directory(al_path_cstr(path, '/'));
	al_destroy_path(path);

    if (!al_install_keyboard()) {
        log_error("Failed to install keyboard");
    }

    if (!al_install_mouse()) {
        log_error("Failed to install mouse");
    }

    if (!al_init_primitives_addon()) {
        log_error("Failed to init primitives addon");
    }

    if (!al_init_image_addon()) {
        log_error("Failed to init image addon");
    }

    al_init_font_addon();
    default_font = al_create_builtin_font();
    if (!default_font) {
        log_error("Failed to create builtin font");
    }

    event_queue = al_create_event_queue();
    if (!event_queue) {
        log_error("Failed to create event queue");
    }

    if (fullscreen) {
        al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW);
    }
    else {
        al_set_new_display_flags(ALLEGRO_WINDOWED);
    }

    display = al_create_display(window_width, window_height);
    if (!display) {
        log_error("Failed to create display @ %dx%d", window_width, window_height);
    }
    al_set_window_title(display, title);

    timer = al_create_timer(1.0 / 60);
    if (!timer) {
        log_error("Failed to create timer");
    }

    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_mouse_event_source());
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_timer_event_source(timer));

    srand(time(NULL));

	// initialize default colors
    black_color       = al_map_rgb(0, 0, 0);
    white_color       = al_map_rgb(255, 255, 255);
	dark_grey_color   = al_map_rgb(64, 64, 64);
	grey_color        = al_map_rgb(128, 128, 128);
	light_grey_color  = al_map_rgb(192, 192, 192);
	red_color         = al_map_rgb(255, 0, 0);
	green_color       = al_map_rgb(0, 255, 0);
	dark_green_color  = al_map_rgb(0, 100, 0);
	blue_color        = al_map_rgb(0, 0, 255);
	yellow_color      = al_map_rgb(255, 255, 0);
	cyan_color        = al_map_rgb(0, 255, 255);
	magenta_color     = al_map_rgb(255, 0, 255);
	maroon_color      = al_map_rgb(128, 0, 0);
	purple_color      = al_map_rgb(128, 0, 128);
	lime_color        = al_map_rgb(191, 255, 0);
	olive_color       = al_map_rgb(128, 128, 0);
	navy_color        = al_map_rgb(0, 0, 128);
	teal_color        = al_map_rgb(0, 128, 128);
	brown_color       = al_map_rgb(101, 55, 0);
}

void destroy_framework()
{
    if (default_font) {
        al_destroy_font(default_font);
        default_font = NULL;
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

void run_game_loop(void (*update_proc)(), void (*render_proc)())
{
    bool should_redraw = true;
    al_start_timer(timer);

    while (!is_done) {
        ALLEGRO_EVENT event;
        al_wait_for_event(event_queue, &event);

        switch (event.type) {
            case ALLEGRO_EVENT_TIMER:
                should_redraw = true;
                if (!is_paused) {
                    update_proc();
                }

                // clear input state
                memset(keys_pressed, false, sizeof(keys_pressed));
                memset(keys_released, false, sizeof(keys_pressed));
                memset(mouse_buttons_pressed, false, sizeof(mouse_buttons_pressed));
                memset(mouse_buttons_released, false, sizeof(mouse_buttons_released));
                mouse_old_x = mouse_x;
                mouse_old_y = mouse_y;
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
                // handle alt-tab
                if ((event.keyboard.modifiers & ALLEGRO_KEYMOD_ALT) &&
                     event.keyboard.keycode == ALLEGRO_KEY_ENTER) {
                    al_set_display_flag(display, ALLEGRO_FULLSCREEN_WINDOW, !(al_get_display_flags(display) & ALLEGRO_FULLSCREEN_WINDOW));
                }
                break;

            case ALLEGRO_EVENT_MOUSE_AXES:
                mouse_x = event.mouse.x;
                mouse_y = event.mouse.y;
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
                if (should_alt_tab_pause) {
                    is_paused = true;
                }
                break;

            case ALLEGRO_EVENT_DISPLAY_SWITCH_IN:
                if (should_alt_tab_pause) {
                    is_paused = false;
                }
                break;
        }

        if (should_redraw && al_is_event_queue_empty(event_queue) && !is_paused) {
            should_redraw = false;
            al_set_target_bitmap(al_get_backbuffer(display));
            al_clear_to_color(al_map_rgb(0, 0, 0));
            render_proc();
            al_flip_display();
        }
    }
}

void quit()
{
    is_done = true;
}

void alt_tab_should_pause(bool true_or_false)
{
    should_alt_tab_pause = true_or_false;
}

int get_window_width()
{
    assert(display != NULL);
    return al_get_display_width(display);
}

int get_window_height()
{
    assert(display != NULL);
    return al_get_display_height(display);
}

bool is_key_down(int keycode)
{
    assert(keycode >= 0 && keycode < ALLEGRO_KEY_MAX);
    return keys[keycode];
}

bool is_key_pressed(int keycode)
{
    assert(keycode >= 0 && keycode < ALLEGRO_KEY_MAX);
    return keys_pressed[keycode];
}

bool is_key_released(int keycode)
{
    assert(keycode >= 0 && keycode < ALLEGRO_KEY_MAX);
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
    return mouse_x - mouse_old_x;
}

int get_mouse_dy()
{
    return mouse_y - mouse_old_y;
}

bool is_mouse_button_down(int mouse_button)
{
    assert(mouse_button >= 0 && mouse_button < al_get_mouse_num_buttons());
    return mouse_buttons[mouse_button];
}

bool is_mouse_button_pressed(int mouse_button)
{
    assert(mouse_button >= 0 && mouse_button < al_get_mouse_num_buttons());
    return mouse_buttons_pressed[mouse_button];
}

bool is_mouse_button_released(int mouse_button)
{
    assert(mouse_button >= 0 && mouse_button < al_get_mouse_num_buttons());
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

bool is_float_equal(float a, float b)
{
	return fabs(a - b) < ALMOST_ZERO;
}

bool is_double_equal(double a, double b)
{
	return fabs(a - b) < ALMOST_ZERO;
}

int lerpi(int a, int b, int alpha)
{
	return a + alpha * (b - a);
}

float lerpf(float a, float b, float alpha)
{
	return a + alpha * (b - a);
}

double lerpd(double a, double b, double alpha)
{
	return a + alpha * (b - a);
}

int get_random_int(int min, int max)
{
    return min + (rand() % (int)(max - min + 1));
}

float get_random_float(float min, float max)
{
    return min + ((float)rand() / ((float)RAND_MAX / (max - min)));
}

bool one_in(int chance)
{
    if (get_random_int(0, chance - 1) == 0)
        return true;
    return false;
}

int roll_dice(int number, int sides)
{
    int result = 0;
    for (int i = 0; i < number; i++)
        result += get_random_int(1, sides);
    return result;
}

ALLEGRO_FONT* get_default_font()
{
    return default_font;
}

float angle_between_points(float x1, float y1, float x2, float y2)
{
    return atan2(y2 - y1, x2 - x1);
}

float angle_between_points_ex(Point p1, Point p2)
{
    return angle_between_points(p1.x, p1.y, p2.x, p2.y);
}

float distance_between_points(float x1, float y1, float x2, float y2)
{
    float dx = x2 - x1;
    float dy = y2 - y1;
    return sqrt(dx * dx + dy * dy);
}

float distance_between_points_ex(Point p1, Point p2)
{
    return distance_between_points(p1->x, p1->y, p2->x, p2->y);
}

bool rectangles_intersect(float l1, float t1, float r1, float b1, float l2, float t2, float r2, float b2)
{
    return !(r1 < l2 || b1 < t2 || l1 > r2 || t1 > b2);
}

bool rectangles_intersect_ex(Rectangle r1, Rectangle r2)
{
    return rectangles_intersect(r1->x, r1-y, r1->x + r1->w, r1->y + r1->h, r2->x, r2-y, r2->x + r2->w, r2->y + r2->h);
}

bool rectangle_contains_point(float l, float t, float r, float b, float x, float y)
{
    return !(x < l || x > r || y < t || y > b);
}

bool rectangle_contains_point_ex(Rectangle r, Point p)
{
    return rectangle_contains_point(r->x, r-y, r->x + r->w, r->y + r->h, p->x, p->y);
}

bool circles_intersect(float x1, float y1, float r1, float x2, float y2, float r2)
{
    float radii = r1 + r2;
    float dx = x2 - x1;
    float dy = y2 - y1;
    return (radii * radii) > (dx * dx + dy * dy);
}

bool circles_intersect_ex(Circle c1, Circle c2)
{
    return circles_intersect(c1->x, c1->y, c1->r, c2->x, c2->y, c2->r);
}

bool circle_contains_point(float x1, float y1, float r, float x2, float y2)
{
    float dx = x2 - x1;
    float dy = y2 - y1;
    return (dx * dx + dy * dy) < (r * r);
}

bool circle_contains_point_ex(Circle c, Point p)
{
    return circle_contains_point(c->x, c->y, c->r, p->x, p->y);
}
