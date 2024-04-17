#include "cylinder.h"

#include "../utilities/uvmapping.h"

#include <cmath>


cylinder::cylinder(std::string _name) : center(0, 0, 0), radius(0), height(0)
{
    name = _name;
        
    // calculate cylinder bounding box for ray optimizations
    bbox = aabb(
        point3(center.x - radius, center.y, center.z - radius),
        point3(center.x + radius, center.y + height, center.z + radius)
    );
}

cylinder::cylinder(point3 _center, double _radius, double _height, std::string _name)
    : center(_center), radius(_radius), height(_height)
{
    name = _name;
        
    // calculate cylinder bounding box for ray optimizations
    bbox = aabb(
        point3(center.x - radius, center.y, center.z - radius),
        point3(center.x + radius, center.y + height, center.z + radius)
    );
}

cylinder::cylinder(point3 _center, double _radius, double _height, std::shared_ptr<material> _material, std::string _name)
    : center(_center), radius(_radius), height(_height), mat(_material)
{
    name = _name;
        
    // calculate cylinder bounding box for ray optimizations
    bbox = aabb(
        point3(center.x - radius, center.y, center.z - radius),
        point3(center.x + radius, center.y + height, center.z + radius)
    );
}

bool cylinder::hit(const ray& r, interval ray_t, hit_record& rec, int depth) const
{
    vector3 oc = r.origin() - center;
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

    double y = r.origin().y + root * r.direction()[1];

    if ((y < center.y) || (y > center.y + height))
    {
        return false;
    }

    rec.t = root;
    rec.hit_point = r.at(rec.t);
    rec.normal = vector3((rec.hit_point.x - center.x) / radius, 0, (rec.hit_point.z - center.z) / radius);
    vector3 outward_normal = (rec.hit_point - center) / radius;
    rec.set_face_normal(r, outward_normal);
    uvmapping::get_cylinder_uv(outward_normal, rec.u, rec.v, radius);
    rec.mat = mat;
    rec.name = name;
    rec.bbox = bbox;

    return true;
}

aabb cylinder::bounding_box() const
{
    return bbox;
}

/// <summary>
/// Update the internal AABB of the mesh.
/// Warning: run this when the mesh is updated.
/// </summary>
void cylinder::updateBoundingBox()
{
    // to implement
}
