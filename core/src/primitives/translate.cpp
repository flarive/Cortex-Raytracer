#include "translate.h"

raytracer::translate::translate(std::shared_ptr<hittable> p, const vector3& displacement)
    : object(p), offset(displacement)
{
    bbox = object->bounding_box() + offset;
}

bool raytracer::translate::hit(const ray& r, interval ray_t, hit_record& rec, int depth) const
{
    // Move the ray backwards by the offset
    ray offset_r(r.origin() - offset, r.direction(), r.time());

    // Determine where (if any) an intersection occurs along the offset ray
    if (!object->hit(offset_r, ray_t, rec, depth))
        return false;

    // Move the intersection point forwards by the offset
    rec.hit_point += offset;

    return true;
}

aabb raytracer::translate::bounding_box() const
{
    return bbox;
}


/// <summary>
/// Update the internal AABB of the mesh.
/// Warning: run this when the mesh is updated.
/// </summary>
void raytracer::translate::updateBoundingBox()
{
    // to implement
}