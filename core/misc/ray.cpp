#include "ray.h"

ray::ray()
{
}

ray::ray(const point3& origin, const vector3& direction) : m_orig(origin), m_dir(direction), m_tm(0)
{
}

ray::ray(const point3& origin, const vector3& direction, double time) : m_orig(origin), m_dir(direction), m_tm(time)
{
}

ray::ray(const point3& origin, const vector3& direction, int _x, int _y, double time) : m_orig(origin), m_dir(direction), x(_x), y(_y), m_tm(time)
{
}

point3 ray::origin() const
{
    return m_orig;
}

vector3 ray::direction() const
{
    return m_dir;
}

double ray::time() const
{
    return m_tm;
}

point3 ray::at(double t) const
{
    return m_orig + t * m_dir;
}

[[nodiscard]] vector3 ray::inverseDirection() const
{
    return 1.0 / m_dir;
}