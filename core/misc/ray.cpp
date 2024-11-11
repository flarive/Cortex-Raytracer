#include "ray.h"

ray::ray()
{
}

ray::ray(const point3& origin, const vector3& direction) : orig(origin), dir(direction), tm(0)
{
}

ray::ray(const point3& origin, const vector3& direction, double time) : orig(origin), dir(direction), tm(time)
{
}

ray::ray(const point3& origin, const vector3& direction, int _x, int _y, double time) : orig(origin), dir(direction), x(_x), y(_y), tm(time)
{
}

point3 ray::origin() const
{
    return orig;
}

vector3 ray::direction() const
{
    return dir;
}

double ray::time() const
{
    return tm;
}

point3 ray::at(double t) const
{
    return orig + t * dir;
}

[[nodiscard]] vector3 ray::inverseDirection() const
{
    return 1.0 / dir;
}