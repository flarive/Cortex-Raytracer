#pragma once

#include "../utilities/types.h"

class ray
{
public:
    ray();
    ray(const point3& origin, const vector3& direction);
    ray(const point3& origin, const vector3& direction, double time);
    ray(const point3& origin, const vector3& direction, int _x, int _y, double time);


    point3 origin() const;
    vector3 direction() const;
    double time() const;
	int x = 0;
	int y = 0;

    point3 at(double t) const;

    [[nodiscard]] vector3 inverseDirection() const;


private:
    point3 orig; // origin of where the ray starts
    vector3 dir; // direction of ray
    double tm = 0; // timestamp of the ray (when it was fired, usefull for motion blur calculation)
};