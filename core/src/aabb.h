#pragma once

#include "constants.h"
#include "utilities/types.h"
#include "utilities/interval.h"
#include "misc/ray.h"

/// <summary>
/// Axis-Aligned Bounding Boxes
/// </summary>
class aabb
{
public:
    interval x, y, z;


    // The default AABB is empty, since intervals are empty by default.
    aabb()
    {
    } 

	aabb(const interval& ix, const interval& iy, const interval& iz)
		: x(ix), y(iy), z(iz) { }

    aabb(const point3& a, const point3& b)
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
    aabb(const aabb& box0, const aabb& box1)
    {
        x = interval(box0.x, box1.x);
        y = interval(box0.y, box1.y);
        z = interval(box0.z, box1.z);
    }


    const interval& axis(int n) const;


    /// <summary>
    /// Expanding bounding box to remove the possibility of numerical problems (for quad primitive for example)
    /// </summary>
    /// <returns></returns>
    aabb pad() const;


    bool hit(const ray& r, interval ray_t) const;


    /**
     * \brief Ray AABB box intersection
     * \param box The AABB box
     * \param ray The Ray
     * \param t Time on the ray of the first hit
     * \return True if the ray intersects the AABB
     */
    static bool rayAABBIntersection(const aabb& box, const ray& ray, double& t);

    /**
     * \brief Ray AABB box intersection
     * \param box The AABB box
     * \param ray The Ray
     * \return True if the ray intersects the AABB
     */
    static bool rayAABBIntersection(const aabb& box, const ray& ray);
};

aabb operator+(const aabb& bbox, const vector3& offset);
aabb operator+(const vector3& offset, const aabb& bbox);
