#include "disk.h"

#include "../utilities/uvmapping.h"

disk::disk()
    : center(vector3(0,0,0)), radius(1.0), height(2.0)
{

}

disk::disk(point3 _center, double _radius, double _height, std::shared_ptr<material> _mat)
    : center(_center), radius(_radius), height(_height), mat(_mat)
{
    bbox = aabb(center - vector3(radius, height / 2, radius), center + vector3(radius, height / 2, radius));
}

bool disk::hit(const ray& r, interval ray_t, hit_record& rec, int depth) const
{
    // Compute the intersection with the plane containing the disk
    double t = (center.y - r.origin().y) / r.direction().y;
    if (t < ray_t.min || t > ray_t.max)
        return false;

    point3 hit_point = r.at(t);

    // Check if the hit point is within the disk's radius
    double dist_squared = (hit_point.x - center.x) * (hit_point.x - center.x) + (hit_point.z - center.z) * (hit_point.z - center.z);
    if (dist_squared > radius * radius)
        return false;

    // Check if the hit point is within the disk's height
    if (hit_point.y < center.y - height / 2 || hit_point.y > center.y + height / 2)
        return false;

    rec.t = t;
    rec.hit_point = r.at(rec.t);
    rec.normal = vector3((rec.hit_point.x - center.x) / radius, 0, (rec.hit_point.z - center.z) / radius);
    vector3 outward_normal = (rec.hit_point - center) / radius;
    rec.set_face_normal(r, outward_normal);
    get_disk_uv(outward_normal, rec.u, rec.v, radius);
    rec.mat = mat;
    rec.name = name;
    rec.bbox = bbox;

    return true;
}

aabb disk::bounding_box() const
{
    return bbox;
}


/// <summary>
/// Update the internal AABB of the mesh.
/// Warning: run this when the mesh is updated.
/// </summary>
void disk::updateBoundingBox()
{
    // to implement
}

