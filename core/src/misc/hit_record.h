#pragma once

#include "../utilities/Types.h"
#include "../aabb.h"
#include "../materials/material.h"

#include <glm/glm.hpp>


class hit_record
{
public:
	point3 hit_point; // point (coordinates) where the hit occurred
	vector3 normal; // normal vector where the hit occurred
	std::shared_ptr<material> mat; // material of the object hit by the ray
	double t;
	double u;
	double v;
	bool front_face; // front-face tracking (object was hit from outside (frontface) or inside (backface) ?)
	std::string name; // name of the object that was hit
	aabb bbox; // bounding box size of the object that was hit

	void set_face_normal(const ray& r, const vector3& outward_normal)
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
};