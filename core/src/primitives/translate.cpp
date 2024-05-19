#include "translate.h"

rt::translate::translate(std::shared_ptr<hittable> p, const vector3& displacement)
    : m_object(p), m_offset(displacement)
{
    m_name = p->getName();
    
    m_bbox = m_object->bounding_box() + m_offset;
}

bool rt::translate::hit(const ray& r, interval ray_t, hit_record& rec, int depth) const
{
    // Move the ray backwards by the offset
    ray offset_r(r.origin() - m_offset, r.direction(), r.time());

    // Determine where (if any) an intersection occurs along the offset ray
    if (!m_object->hit(offset_r, ray_t, rec, depth))
        return false;

    // Move the intersection point forwards by the offset
    rec.hit_point += m_offset;

    return true;
}

aabb rt::translate::bounding_box() const
{
    return m_bbox;
}


/// <summary>
/// Update the internal AABB of the mesh.
/// Warning: run this when the mesh is updated.
/// </summary>
void rt::translate::updateBoundingBox()
{
    // to implement
}