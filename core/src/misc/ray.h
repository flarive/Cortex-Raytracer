#ifndef RAY_H
#define RAY_H

#include <glm/glm.hpp>
#include "../utilities/types.h"

class ray
{
public:
    ray() {}

    ray(const Point3& origin, const Vec3& direction) : orig(origin), dir(direction), tm(0)
    {}

    ray(const Point3& origin, const Vec3& direction, double time = 0.0)
        : orig(origin), dir(direction), tm(time)
    {}


    Point3 origin() const { return orig; }
    Vec3 direction() const { return dir; }
    double time() const { return tm; }

    Point3 at(double t) const
    {
        return orig + t * dir;
    }

    [[nodiscard]] Vec3 inverseDirection() const
    {
        return 1.0 / dir;
    }


private:
    Point3 orig; // origin of where the ray starts
    Vec3 dir; // direction of ray
    double tm; // timestamp of the ray (when it was fired, usefull for motion blur calculation)
};

#endif