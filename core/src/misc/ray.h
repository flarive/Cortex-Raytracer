#ifndef RAY_H
#define RAY_H

#include <glm/glm.hpp>
#include "../utilities/types.h"

class ray
{
public:
    ray() {}

    ray(const point3& origin, const vector3& direction) : orig(origin), dir(direction), tm(0)
    {}

    ray(const point3& origin, const vector3& direction, double time)
        : orig(origin), dir(direction), tm(time)
    {}


    point3 origin() const { return orig; }
    vector3 direction() const { return dir; }
    double time() const { return tm; }

    point3 at(double t) const
    {
        return orig + t * dir;
    }

    [[nodiscard]] vector3 inverseDirection() const
    {
        return 1.0 / dir;
    }


private:
    point3 orig; // origin of where the ray starts
    vector3 dir; // direction of ray
    double tm; // timestamp of the ray (when it was fired, usefull for motion blur calculation)
};

#endif