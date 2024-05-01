#include "aabb.h"


aabb::aabb()
{
}


aabb::aabb(const interval& ix, const interval& iy, const interval& iz) : x(ix), y(iy), z(iz)
{
}

aabb::aabb(const vector3& a, const vector3& b)
{
    // Treat the two points a and b as extrema for the bounding box, so we don't require a
    // particular minimum/maximum coordinate order.
    x = interval(fmin(a[0], b[0]), fmax(a[0], b[0]));
    y = interval(fmin(a[1], b[1]), fmax(a[1], b[1]));
    z = interval(fmin(a[2], b[2]), fmax(a[2], b[2]));
}

/// <summary>
/// Bounding box of sub bounding boxes
/// </summary>
/// <param name="box0"></param>
/// <param name="box1"></param>
aabb::aabb(const aabb& box0, const aabb& box1)
{
    x = interval(box0.x, box1.x);
    y = interval(box0.y, box1.y);
    z = interval(box0.z, box1.z);
}


const interval& aabb::axis(int n) const
{
    if (n == 1) return y;
    if (n == 2) return z;
    return x;
}

aabb aabb::pad() const
{
    // Return an AABB that has no side narrower than some delta, padding if necessary.
    double delta = 0.0001;
    interval new_x = (x.size() >= delta) ? x : x.expand(delta);
    interval new_y = (y.size() >= delta) ? y : y.expand(delta);
    interval new_z = (z.size() >= delta) ? z : z.expand(delta);

    return aabb(new_x, new_y, new_z);
}

vector3 aabb::min() const
{
    return vector3(x.min, y.min, z.min);
}

vector3 aabb::max() const
{
    return vector3(x.max, y.max, z.max);
}

bool aabb::hit(const ray& r, interval ray_t) const
{
    for (int a = 0; a < 3; a++)
    {
        auto invD = 1 / r.direction()[a];
        auto orig = r.origin()[a];

        auto t0 = (axis(a).min - orig) * invD;
        auto t1 = (axis(a).max - orig) * invD;

        if (invD < 0)
            std::swap(t0, t1);

        if (t0 > ray_t.min) ray_t.min = t0;
        if (t1 < ray_t.max) ray_t.max = t1;

        if (ray_t.max <= ray_t.min)
            return false;
    }
    return true;
}

aabb operator+(const aabb& bbox, const vector3& offset)
{
    return aabb(bbox.x + offset.x, bbox.y + offset.y, bbox.z + offset.z);
}

aabb operator+(const vector3& offset, const aabb& bbox)
{
    return bbox + offset;
}

aabb operator*(const aabb& bbox, const vector3& offset)
{
    return aabb(bbox.x * offset.x, bbox.y * offset.y, bbox.z * offset.z);
}

aabb operator*(const vector3& offset, const aabb& bbox)
{
    return bbox * offset;
}

/**
 * \brief Ray AABB box intersection
 * \param box The AABB box
 * \param ray The Ray
 * \param t Time on the ray of the first hit
 * \return True if the ray intersects the AABB
 */
bool aabb::rayAABBIntersection(const aabb& box, const ray& ray, double& t)
{
    // Source: https://www.scratchapixel.com
    // A Minimal Ray-Tracer: Rendering Simple Shapes (Sphere, Cube, Disk, Plane, etc.)
    const auto invDirection = ray.inverseDirection();
    double txmin, txmax, tymin, tymax, tzmin, tzmax;

    if (invDirection.x >= 0.0)
    {
        txmin = (box.x.min - ray.origin().x) * invDirection.x;
        txmax = (box.x.max - ray.origin().x) * invDirection.x;
    }
    else
    {
        txmin = (box.x.max - ray.origin().x) * invDirection.x;
        txmax = (box.x.min - ray.origin().x) * invDirection.x;
    }

    if (invDirection.y >= 0.0)
    {
        tymin = (box.y.min - ray.origin().y) * invDirection.y;
        tymax = (box.y.max - ray.origin().y) * invDirection.y;
    }
    else
    {
        tymin = (box.y.max - ray.origin().y) * invDirection.y;
        tymax = (box.y.min - ray.origin().y) * invDirection.y;
    }

    if ((txmin > tymax) || (tymin > txmax))
        return false;

    if (tymin > txmin)
        txmin = tymin;

    if (tymax < txmax)
        txmax = tymax;

    if (invDirection.z >= 0.0)
    {
        tzmin = (box.z.min - ray.origin().z) * invDirection.z;
        tzmax = (box.z.max - ray.origin().z) * invDirection.z;
    }
    else
    {
        tzmin = (box.z.max - ray.origin().z) * invDirection.z;
        tzmax = (box.z.min - ray.origin().z) * invDirection.z;
    }

    if ((txmin > tzmax) || (tzmin > txmax))
        return false;

    if (tzmin > txmin)
        txmin = tzmin;

    if (tzmax < txmax)
        txmax = tzmax;

    t = txmin;
    if (t < 0)
    {
        t = txmax;
        if (t < 0)
        {
            return false;
        }
    }

    return true;
}

/**
 * \brief Ray AABB box intersection
 * \param box The AABB box
 * \param ray The Ray
 * \return True if the ray intersects the AABB
 */
bool aabb::rayAABBIntersection(const aabb& box, const ray& ray)
{
    double t;
    return rayAABBIntersection(box, ray, t);
}
