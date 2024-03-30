#include "ray.h"


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