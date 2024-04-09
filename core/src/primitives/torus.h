#pragma once

#include "../constants.h"
#include "hittable.h"
#include "hittable_list.h"
#include "../misc/ray.h"
#include "../aabb.h"
#include "../utilities/interval.h"
#include "../misc/hit_record.h"
#include "../utilities/math_utils.h"

#include <cmath>


class torus : public hittable
{
public:
	torus() {}
	torus(vector3 center, float majorRadius, float minorRadius)
		: center(center), majorRadius(majorRadius), minorRadius(minorRadius)
	{
	
	}

	bool hit(const ray& ray, float t_min, float t_max, hit_record& rec) const
	{
		vector3 oc = ray.origin() - center;
		float a = vector_length_squared(ray.direction());
		float b = 2.0 * dot(oc, ray.direction());
		float c = length_squared(oc) - majorRadius * majorRadius - minorRadius * minorRadius;
		float discriminant = b * b - 4 * a * c;

		if (discriminant > 0) {
			float root = sqrt(discriminant);
			float temp = (-b - root) / (2.0 * a);
			if (temp < t_max && temp > t_min) {
				rec.t = temp;
				rec.p = ray.at(rec.t);
				vector3 outward_normal = (rec.p - center).unit();
				rec.set_face_normal(ray, outward_normal);
				return true;
			}
			temp = (-b + root) / (2.0 * a);
			if (temp < t_max && temp > t_min) {
				rec.t = temp;
				rec.p = ray.at(rec.t);
				vector3 outward_normal = (rec.p - center).unit();
				rec.set_face_normal(ray, outward_normal);
				return true;
			}
		}
		return false;
	}

private:
	vector3 center;
	float majorRadius;
	float minorRadius;
};

