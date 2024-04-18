#include "hit_record.h"


void hit_record::set_face_normal(const ray& r, const vector3& outward_normal)
{
	// Sets the hit record normal vector.
	// NOTE: the parameter `outward_normal` is assumed to have unit length.

	if (glm::dot(r.direction(), outward_normal) > 0.0)
	{
		// ray is inside the hittable primitive
		normal = -outward_normal;
		front_face = false;
	}
	else
	{
		// ray is outside the hittable primitive
		normal = outward_normal;
		front_face = true;
	}
}