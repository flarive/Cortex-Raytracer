#ifndef HITTABLE_H
#define HITTABLE_H

#include "ray.h"
#include "interval.h"

class hit_record
{
public:
    point3 p;
    vec3 normal;
    double t;
    bool front_face; // front-face tracking

    void set_face_normal(const ray& r, const vec3& outward_normal)
    {
        // Sets the hit record normal vector.
        // NOTE: the parameter `outward_normal` is assumed to have unit length.

        if (dot(r.direction(), outward_normal) > 0.0)
        {
            // ray is inside the hittable primitive
            normal = -outward_normal;
            front_face = false;
        }
        else
        {
            // ray is outside the hittable primitive
            normal = outward_normal;
            front_face = true;
        }
    }
};

class hittable
{
public:
    virtual ~hittable() = default;

    // virtual hit method that needs to be implemented for all primitives
    // because each primitive has it own intersection calculation logic
    virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const = 0;
};

#endif