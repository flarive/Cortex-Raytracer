#pragma once

#include "../utilities/types.h"

class ray
{
public:
    ray();
    ray(const point3& origin, const vector3& direction);
    ray(const point3& origin, const vector3& direction, double time);


    point3 origin() const;
    vector3 direction() const;
    double time() const;

    point3 at(double t) const;

    [[nodiscard]] vector3 inverseDirection() const;


private:
    point3 orig; // origin of where the ray starts
    vector3 dir; // direction of ray
    double tm = 0; // timestamp of the ray (when it was fired, usefull for motion blur calculation)
};