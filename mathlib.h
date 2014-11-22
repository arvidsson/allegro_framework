#ifndef INCLUDED_MATHLIB_H
#define INCLUDED_MATHLIB_H

#ifdef __cplusplus
   extern "C" {
#endif

// Math Constants.
#define PI (3.14159265358979323846)
#define TWO_PI (PI * 2)
#define HALF_PI (PI / 2)

// Convert to and from degrees and radians.
#define DEG2RAD(DEG) ((DEG)*((PI)/(180.0)))
#define RAD2DEG(RAD) ((RAD)*((180.0)/(PI)))

// A point/vector.
typedef struct {
    float x, y;
} Point;

// A rectangle.
typedef struct {
    float x, y;
    float w, h;
} Rectangle;

// A circle.
typedef struct {
    float x, y;
    float r;
} Circle;

// A velocity.
typedef struct {
    float dx, dy;
} Velocity;

// Returns the angle between two points.
float angle_between_points(float x1, float y1, float x2, float y2);

// Returns the distance between two points.
float distance_between_points(float x1, float y1, float x2, float y2);

// Returns true if two rectangles overlap.
bool rectangles_intersect(float l1, float t1, float r1, float b1, float l2, float t2, float r2, float b2);

// Returns true if the point is inside the rectangle.
bool rectangle_contains_point(float l, float t, float r, float b, float x, float y);

// Returns true if two circles overlap.
bool circles_intersect(float x1, float y1, float r1, float x2, float y2, float r2);

// Returns true if the point is inside the circle.
bool circle_contains_point(float x1, float y1, float r, float x2, float y2);

// Variants of the functions using the structs defined above.
float distance_between_points_ex(const Point* p1, const Point* p2);
bool rectangles_intersect_ex(const Rectangle* r1, const Rectangle* r2);
bool rectangle_contains_point_ex(const Rectangle* r, const Point* p);
bool circles_intersect_ex(const Circle* c1, const Circle* c2);
bool circle_contains_point_ex(const Circle* c, const Point* p);

#ifdef __cplusplus
   }
#endif

#endif
