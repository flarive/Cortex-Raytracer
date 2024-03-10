#ifndef CYLINDER_H
#define CYLINDER_H

#include "../constants.h"
#include "hittable.h"
#include "hittable_list.h"
#include "../misc/point3.h"
#include "../misc/ray.h"
#include "../misc/vec3.h"
#include "../aabb.h"
#include "../interval.h"

#include <cmath>

/// <summary>
/// Cylinder primitive
/// </summary>
class cylinder : public hittable
{
public:

    cylinder() : center(0, 0, 0), radius(0), height(0)
    {
        // calculate cylinder bounding box for ray optimizations
        bbox = aabb(
            point3(center.x() - radius, center.y(), center.z() - radius),
            point3(center.x() + radius, center.y() + height, center.z() + radius)
        );
    }

    cylinder(point3 _center, double _radius, double _height)
        : center(_center), radius(_radius), height(_height)
    {
        // calculate cylinder bounding box for ray optimizations
        bbox = aabb(
            point3(center.x() - radius, center.y(), center.z() - radius),
            point3(center.x() + radius, center.y() + height, center.z() + radius)
        );
    }

    cylinder(point3 _center, double _radius, double _height, std::shared_ptr<material> _material)
        : center(_center), radius(_radius), height(_height), mat(_material)
    {
        // calculate cylinder bounding box for ray optimizations
        bbox = aabb(
            point3(center.x() - radius, center.y(), center.z() - radius),
            point3(center.x() + radius, center.y() + height, center.z() + radius)
        );
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const
    {
        vec3 oc = r.origin() - center;
        double a = r.direction()[0] * r.direction()[0] + r.direction()[2] * r.direction()[2];
        double b = 2.0 * (oc[0] * r.direction()[0] + oc[2] * r.direction()[2]);
        double c = oc[0] * oc[0] + oc[2] * oc[2] - radius * radius;
        double d = b * b - 4 * a * c;

        if (d < 0)
        {
            return false;
        }

        double root = (-b - sqrt(d)) / (2.0 * a);

        if (root < ray_t.min || ray_t.max < root)
        {
            root = (-b + sqrt(d)) / (2.0 * a);
            if (root < ray_t.min || ray_t.max < root)
            {
                return false;
            }
        }

        double y = r.origin().y() + root * r.direction()[1];

        if ((y < center.y()) || (y > center.y() + height))
        {
            return false;
        }

        rec.t = root;
        rec.p = r.at(rec.t);
        rec.normal = vec3((rec.p.x() - center.x()) / radius, 0, (rec.p.z() - center.z()) / radius);
        vec3 outward_normal = (rec.p - center) / radius;
        rec.set_face_normal(r, outward_normal);
        get_cylinder_uv(outward_normal, rec.u, rec.v, radius);
        rec.mat = this->mat;
        return true;
    }

    aabb bounding_box() const override
    {
        return bbox;
    }

    

private:
    point3 center;
    double radius;
    double height;
    std::shared_ptr<material> mat;
    aabb bbox; // bounding box

    void get_cylinder_uv(const vec3& p, double& u, double& v, double radius) const
    {
        auto theta = std::atan2(p[0], p[2]);
        auto phi = std::atan2(p[1], radius);
        u = 1 - (theta + M_PI) / (2 * M_PI);
        v = (phi + M_PI / 2) / M_PI;
    }
};

#endif