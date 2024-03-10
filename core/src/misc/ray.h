#ifndef RAY_H
#define RAY_H

#include "vec3.h"
#include "point3.h"

class ray
{
public:
    ray() {}

    ray(const point3& origin, const vec3& direction) : orig(origin), dir(direction), tm(0)
    {}

    ray(const point3& origin, const vec3& direction, double time = 0.0)
        : orig(origin), dir(direction), tm(time)
    {}


    point3 origin() const { return orig; }
    vec3 direction() const { return dir; }
    double time() const { return tm; }

    point3 at(double t) const
    {
        return orig + t * dir;
    }

private:
    point3 orig; // origin of where the ray starts
    vec3 dir; // direction of ray
    double tm; // timestamp of the ray (when it was fired, usefull for motion blur calculation)
};

#endif