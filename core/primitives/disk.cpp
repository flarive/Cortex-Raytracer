#include "disk.h"

#include "../utilities/uvmapping.h"

disk::disk(std::string _name)
    : disk(vector3(0,0,0), 1.0, 2.0, nullptr, uvmapping(), _name)
{
}

disk::disk(point3 _center, double _radius, double _height, std::shared_ptr<material> _mat, std::string _name)
    : disk(_center, _radius, _height, _mat, uvmapping(), _name)
{
}

disk::disk(point3 _center, double _radius, double _height, std::shared_ptr<material> _mat, const uvmapping& _mapping, std::string _name)
    : center(_center), radius(_radius), height(_height), mat(_mat)
{
    m_name = _name;
    m_mapping = _mapping;

    // calculate disk bounding box for ray optimizations
    m_bbox = aabb(center - vector3(radius, height / 2, radius), center + vector3(radius, height / 2, radius));
}

bool disk::hit(const ray& r, interval ray_t, hit_record& rec, int depth, randomizer& rnd) const
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

    rec.t = t;
    rec.hit_point = hit_point;

    vector3 outward_normal = glm::normalize(vector3(0, 1, 0)); // Disk is in the XY plane, normal is in Y direction

    rec.set_face_normal(r, outward_normal);

    // Transform the hit point into the local disk coordinates
    vector3 local_hit_point = hit_point - center;

    get_disk_uv(local_hit_point, rec.u, rec.v, radius, m_mapping);

    rec.mat = mat;
    rec.name = m_name;
    rec.bbox = m_bbox;

    return true;
}

aabb disk::bounding_box() const
{
    return m_bbox;
}

/// <summary>
/// Update the internal AABB of the mesh.
/// Warning: run this when the mesh is updated.
/// </summary>
void disk::updateBoundingBox()
{
    // to implement
}