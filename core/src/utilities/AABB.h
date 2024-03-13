#pragma once

#include "../misc/ray.h"
#include "Types.h"

#include <algorithm>

class AABB
{
public:
    AABB() = default;

    //explicit AABB(const vec3& a, const vec3& b);

    explicit AABB(const vec3& a, const vec3& b) :
        m_start(std::min(a.x(), b.x()),
            std::min(a.y(), b.y()),
            std::min(a.z(), b.z())),
        m_end(std::max(a.x(), b.x()),
            std::max(a.y(), b.y()),
            std::max(a.z(), b.z()))
    {

    }


    /**
     * \brief Return the minimum coordinate on the X axis
     * \return The minimum coordinate on the X axis
     */
    double minX() const { return m_start.x(); }

    /**
     * \brief Return the minimum coordinate on the Y axis
     * \return The minimum coordinate on the Y axis
     */
    double minY() const { return m_start.y(); }

    /**
     * \brief Return the minimum coordinate on the Z axis
     * \return The minimum coordinate on the Z axis
     */
    double minZ() const { return m_start.z(); }

    /**
     * \brief Return the point whose coordinates are minimum
     * \return The point whose coordinates are minimum
     */
    const vec3& min() const { return m_start; }

    /**
     * \brief Return the maximum coordinate on the X axis
     * \return The maximum coordinate on the X axis
     */
    double maxX() const { return m_end.x(); }

    /**
     * \brief Return the maximum coordinate on the Y axis
     * \return The maximum coordinate on the Y axis
     */
    double maxY() const { return m_end.y(); }

    /**
     * \brief Return the maximum coordinate on the Z axis
     * \return The maximum coordinate on the Z axis
     */
    double maxZ() const { return m_end.z(); }

    /**
     * \brief Return the point whose coordinates are maximum
     * \return The point whose coordinates are maximum
     */
    const vec3& max() const { return m_end; }

    /**
     * \brief Return the size on the X axis
     * \return The size on the X axis
     */
    double sizeX() const { return m_end.x() - m_start.x(); }

    /**
     * \brief Return the size on the Y axis
     * \return The size on the Y axis
     */
    double sizeY() const { return m_end.y() - m_start.y(); }

    /**
     * \brief Return the size on the Z axis
     * \return The size on the Z axis
     */
    double sizeZ() const { return m_end.z() - m_start.z(); }

    /**
     * \brief Return the center of the AABB
     * \return The center of the AABB
     */
    [[nodiscard]] vec3 center() const { return (m_start + m_end) / 2.0; }

private:

    /**
     * \brief Minimum coordinates of the AABB
     */
    vec3 m_start;

    /**
     * \brief Maximum coordinates of the AABB
     */
    vec3 m_end;
};

/**
 * \brief Ray AABB box intersection
 * \param box The AABB box
 * \param ray The Ray
 * \param t Time on the ray of the first hit
 * \return True if the ray intersects the AABB
 */
bool rayAABBIntersection(const AABB& box, const ray& ray, double& t)
{
    // Source: https://www.scratchapixel.com
    //         A Minimal Ray-Tracer: Rendering Simple Shapes (Sphere, Cube, Disk, Plane, etc.)

    const auto invDirection = ray.inverseDirection();
    double txmin, txmax, tymin, tymax, tzmin, tzmax;

    if (invDirection.x() >= 0.0)
    {
        txmin = (box.minX() - ray.origin().x()) * invDirection.x();
        txmax = (box.maxX() - ray.origin().x()) * invDirection.x();
    }
    else {
        txmin = (box.maxX() - ray.origin().x()) * invDirection.x();
        txmax = (box.minX() - ray.origin().x()) * invDirection.x();
    }

    if (invDirection.y() >= 0.0)
    {
        tymin = (box.minY() - ray.origin().y()) * invDirection.y();
        tymax = (box.maxY() - ray.origin().y()) * invDirection.y();
    }
    else {
        tymin = (box.maxY() - ray.origin().y()) * invDirection.y();
        tymax = (box.minY() - ray.origin().y()) * invDirection.y();
    }

    if ((txmin > tymax) || (tymin > txmax))
        return false;

    if (tymin > txmin)
        txmin = tymin;

    if (tymax < txmax)
        txmax = tymax;

    if (invDirection.z() >= 0.0)
    {
        tzmin = (box.minZ() - ray.origin().z()) * invDirection.z();
        tzmax = (box.maxZ() - ray.origin().z()) * invDirection.z();
    }
    else {
        tzmin = (box.maxZ() - ray.origin().z()) * invDirection.z();
        tzmax = (box.minZ() - ray.origin().z()) * invDirection.z();
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
bool rayAABBIntersection(const AABB& box, const ray& ray)
{
    double t;
    return rayAABBIntersection(box, ray, t);
}
