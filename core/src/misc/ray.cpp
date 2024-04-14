#include "ray.h"


//#include <eigen/eigen/StdVector>
//#include <eigen/eigen/Geometry>
//#include <eigen/eigen/core>
//#include <eigen/eigen/dense>

ray::ray()
{
}

ray::ray(const point3& origin, const vector3& direction) : orig(origin), dir(direction), tm(0)
{
    //dir2 = Vector3d(direction.x, direction.y, direction.z);
    //inv = dir2.cwiseInverse();

    //sign = (inv.array() < 0).cast<int>();
}

ray::ray(const point3& origin, const vector3& direction, double time) : orig(origin), dir(direction), tm(time)
{
    //dir2 = Vector3d(direction.x, direction.y, direction.z);
    //inv = dir2.cwiseInverse();

    //sign = (inv.array() < 0).cast<int>();
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