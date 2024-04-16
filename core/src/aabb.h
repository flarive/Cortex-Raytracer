#pragma once

#include "utilities/types.h"
#include "utilities/interval.h"
#include "misc/ray.h"

/// <summary>
/// Axis-Aligned Bounding Boxes
/// </summary>
class aabb
{
public:
    // The default AABB is empty, since intervals are empty by default.
    aabb();
    aabb(const interval& ix, const interval& iy, const interval& iz);
    aabb(const vector3& a, const vector3& b);

    /// <summary>
    /// Bounding box of sub bounding boxes
    /// </summary>
    /// <param name="box0"></param>
    /// <param name="box1"></param>
    aabb(const aabb& box0, const aabb& box1);


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

    interval x, y, z;
};

aabb operator+(const aabb& bbox, const vector3& offset);
aabb operator+(const vector3& offset, const aabb& bbox);

aabb operator*(const aabb& bbox, const vector3& offset);
aabb operator*(const vector3& offset, const aabb& bbox);