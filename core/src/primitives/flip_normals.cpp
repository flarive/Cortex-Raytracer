#include "flip_normals.h"


flip_normals::flip_normals(std::shared_ptr<hittable> p) : object(p)
{
    bbox = object->bounding_box();
}

bool flip_normals::hit(const ray& r, interval ray_t, hit_record& rec, int depth) const
{
    if (object->hit(r, ray_t, rec, depth))
    {
        rec.normal = -rec.normal;
        return true;
    }
    else
        return false;
}

aabb flip_normals::bounding_box() const
{
    return bbox;
}

/// <summary>
/// Update the internal AABB of the mesh.
/// Warning: run this when the mesh is updated.
/// </summary>
void flip_normals::updateBoundingBox()
{
    // to implement
}