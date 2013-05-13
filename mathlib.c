#include "mathlib.h"
#include <time.h>
#include <math.h>

void init_rng()
{
    srand(time(NULL));
}

int get_random_int(int min, int max)
{
    return min + (rand() % (int)(max - min + 1));
}

float get_random_float(float min, float max)
{
    return min + ((float)rand() / ((float)RAND_MAX / (max - min)));
}

float distance_between_points(float x1, float y1, float x2, float y2)
{
    float dx = x2 - x1;
    float dy = y2 - y1;
    return sqrt(dx * dx + dy * dy);
}

bool rectangles_intersect(float l1, float t1, float r1, float b1, float l2, float t2, float r2, float b2)
{
    return !(r1 < l2 || b1 < t2 || l1 > r2 || t1 > b2);
}

bool rectangle_contains_point(float l, float t, float r, float b, float x, float y)
{
    return !(x < l || x > r || y < t || y > b);
}

bool circles_intersect(float x1, float y1, float r1, float x2, float y2, float r2)
{
    float radii = r1 + r2;
    float dx = x2 - x1;
    float dy = y2 - y1;
    return (radii * radii) > (dx * dx + dy * dy);
}

bool circle_contains_point(float x1, float y1, float r, float x2, float y2)
{
    float dx = x2 - x1;
    float dy = y2 - y1;
    return (dx * dx + dy * dy) < (r * r);
}
