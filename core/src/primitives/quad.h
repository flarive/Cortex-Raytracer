#ifndef QUAD_H
#define QUAD_H

#include "../constants.h"
#include "hittable.h"
#include "hittable_list.h"

#include <glm/glm.hpp>
#include "../utilities/types.h"

#include <cmath>

/// <summary>
/// Quadrilateral (plane) primitive
/// </summary>
class quad : public hittable
{
public:
    quad(const Point3& _Q, const Vec3& _u, const Vec3& _v, shared_ptr<material> _material)
        : Q(_Q), u(_u), v(_v), mat(_material)
    {
        auto n = cross2(u, v);
        normal = unit_vector(n);
        D = dot2(normal, Q);
        w = n / dot2(n, n);
        
        set_bounding_box();
    }

    virtual void set_bounding_box()
    {
        bbox = aabb(Q, Q + u + v).pad();
    }

    aabb bounding_box() const override
    {
        return bbox;
    }

    

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override
    {
        auto denom = dot2(normal, r.direction());

        // No hit if the ray is parallel to the plane.
        if (fabs(denom) < 1e-8)
            return false;

        // Return false if the hit point parameter t is outside the ray interval.
        auto t = (D - dot2(normal, r.origin())) / denom;
        if (!ray_t.contains(t))
            return false;

        // Determine the hit point lies within the planar shape using its plane coordinates.
        auto intersection = r.at(t);
        Vec3 planar_hitpt_vector = intersection - Q;
        auto alpha = dot2(w, cross2(planar_hitpt_vector, v));
        auto beta = dot2(w, cross2(u, planar_hitpt_vector));

        if (!is_interior(alpha, beta, rec))
            return false;

        // Ray hits the 2D shape; set the rest of the hit record and return true.
        rec.t = t;
        rec.p = intersection;
        rec.mat = mat;
        rec.set_face_normal(r, normal);

        return true;
    }

    virtual bool is_interior(double a, double b, hit_record& rec) const
    {
        // Given the hit point in plane coordinates, return false if it is outside the
        // primitive, otherwise set the hit record UV coordinates and return true.

        if ((a < 0) || (1 < a) || (b < 0) || (1 < b))
            return false;

        rec.u = a;
        rec.v = b;
        return true;
    }

    

private:
    Point3 Q;
    Vec3 u, v;
    shared_ptr<material> mat;
    aabb bbox; // bounding box
    Vec3 normal;
    double D;
    Vec3 w; // constant cached value
};

inline shared_ptr<hittable_list> box(const Point3& a, const Point3& b, shared_ptr<material> mat)
{
    // Returns the 3D box (six sides) that contains the two opposite vertices a & b.

    auto sides = make_shared<hittable_list>();

    // Construct the two opposite vertices with the minimum and maximum coordinates.
    auto min = Point3(fmin(a.x, b.x), fmin(a.y, b.y), fmin(a.z, b.z));
    auto max = Point3(fmax(a.x, b.x), fmax(a.y, b.y), fmax(a.z, b.z));

    auto dx = Vec3(max.x - min.x, 0, 0);
    auto dy = Vec3(0, max.y - min.y, 0);
    auto dz = Vec3(0, 0, max.z - min.z);

    sides->add(make_shared<quad>(Point3(min.x, min.y, max.z), dx, dy, mat)); // front
    sides->add(make_shared<quad>(Point3(max.x, min.y, max.z), -dz, dy, mat)); // right
    sides->add(make_shared<quad>(Point3(max.x, min.y, min.z), -dx, dy, mat)); // back
    sides->add(make_shared<quad>(Point3(min.x, min.y, min.z), dz, dy, mat)); // left
    sides->add(make_shared<quad>(Point3(min.x, max.y, max.z), dx, -dz, mat)); // top
    sides->add(make_shared<quad>(Point3(min.x, min.y, min.z), dx, dz, mat)); // bottom

    return sides;
}

#endif