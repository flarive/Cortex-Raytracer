#include "scale.h"

rt::scale::scale(std::shared_ptr<hittable> p, const vector3& _scale) : m_object(p), m_scale(_scale)
{
    m_bbox = m_object->bounding_box() * m_scale;
}

bool rt::scale::hit(const ray& r, interval ray_t, hit_record& rec, int depth) const
{
	vector3 origin = r.origin();
	origin = origin * vector3(1 / m_scale.x, 1 / m_scale.y, 1 / m_scale.z);



	ray scaled_r = ray(origin, r.direction(), r.time());
	if (m_object->hit(scaled_r, ray_t, rec, depth))
	{
		rec.hit_point = rec.hit_point * m_scale;
        return true;
	}

    return false;
}

aabb rt::scale::bounding_box() const
{
    return m_bbox;
}

/// <summary>
/// Update the internal AABB of the mesh.
/// Warning: run this when the mesh is updated.
/// </summary>
void rt::scale::updateBoundingBox()
{
}