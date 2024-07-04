#include "scale.h"

#include "../utilities//randomizer.h"

rt::scale::scale(std::shared_ptr<hittable> p, const vector3& _scale)
	: m_object(p), m_scale(_scale)
{
	m_name = p->getName();

	// Calculate new bounding box after scaling
	m_bbox = m_object->bounding_box(); // Get original bounding box

	// Apply scaling to the bounding box
	m_bbox.x.min *= m_scale.x;
	m_bbox.x.max *= m_scale.x;

	m_bbox.y.min *= m_scale.y;
	m_bbox.y.max *= m_scale.y;

	m_bbox.z.min *= m_scale.z;
	m_bbox.z.max *= m_scale.z;
}

bool rt::scale::hit(const ray& r, interval ray_t, hit_record& rec, int depth) const
{
	// Apply scaling to ray's origin and direction
	vector3 origin = r.origin() / m_scale;
	vector3 direction = r.direction() / m_scale;

	ray scaled_r = ray(origin, direction, r.time());
	if (m_object->hit(scaled_r, ray_t, rec, depth))
	{
		// Scale hit point and normal back to the original scale
		rec.hit_point *= m_scale;

		// Calculate normal in the scaled space
		vector3 normal = rec.normal / m_scale;
		rec.normal = unit_vector(normal);

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