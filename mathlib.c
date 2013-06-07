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

float distance_between_points_ex(const point* p1, const point* p2)
{
    return distance_between_points(p1->x, p1->y, p2->x, p2->y);
}

bool rectangles_intersect(float l1, float t1, float r1, float b1, float l2, float t2, float r2, float b2)
{
    return !(r1 < l2 || b1 < t2 || l1 > r2 || t1 > b2);
}

bool rectangles_intersect_ex(const rect* r1, const rect* r2)
{
    return rectangles_intersect(r1->x, r1-y, r1->x + r1->w, r1->y + r1->h, r2->x, r2-y, r2->x + r2->w, r2->y + r2->h);
}

bool rectangle_contains_point(float l, float t, float r, float b, float x, float y)
{
    return !(x < l || x > r || y < t || y > b);
}

bool rectangle_contains_point_ex(const rect* r, const point* p)
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

bool circles_intersect_ex(const circle* c1, const circle* c2)
{
    return circles_intersect(c1->x, c1->y, c1->r, c2->x, c2->y, c2->r);
}

bool circle_contains_point(float x1, float y1, float r, float x2, float y2)
{
    float dx = x2 - x1;
    float dy = y2 - y1;
    return (dx * dx + dy * dy) < (r * r);
}

bool circle_contains_point_ex(const circle* c, const point* p)
{
    return circle_contains_point(c->x, c->y, c->r, p->x, p->y);
}
