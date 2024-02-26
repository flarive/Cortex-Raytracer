#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"

class vec3;
class material;

class sphere : public hittable
{
public:
    sphere(point3 _center, double _radius)
        : center(_center), radius(_radius), mat(nullptr) {}
    
    sphere(point3 _center, double _radius, shared_ptr<material> _material)
        : center(_center), radius(_radius), mat(_material) {}

    

    /// <summary>
    /// Logic of sphere ray hit detection
    /// </summary>
    /// <param name="r"></param>
    /// <param name="ray_t"></param>
    /// <param name="rec"></param>
    /// <returns></returns>
    bool hit(const ray& r, interval ray_t, hit_record& rec) const override
    {
        vec3 oc = r.origin() - center;
        auto a = r.direction().length_squared();
        auto half_b = dot(oc, r.direction());
        auto c = oc.length_squared() - radius * radius;

        auto discriminant = half_b * half_b - a * c;
        if (discriminant < 0) return false;
        auto sqrtd = sqrt(discriminant);

        // Find the nearest root that lies in the acceptable range.
        auto root = (-half_b - sqrtd) / a;
        if (!ray_t.surrounds(root)) {
            root = (-half_b + sqrtd) / a;
            if (!ray_t.surrounds(root))
                return false;
        }

        // number of hits encountered by the ray (only the nearest ?)
        rec.t = root;

        // point coordinate of the hit
        rec.p = r.at(rec.t);

        // material of the hit object
        rec.mat = mat;

        // set normal and front-face tracking
        vec3 outward_normal = (rec.p - center) / radius;
        rec.set_face_normal(r, outward_normal);

        return true;
    }

private:
    point3 center;
    double radius;
    shared_ptr<material> mat;
};

#endif