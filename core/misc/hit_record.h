#pragma once

#include "../misc/aabb.h"
#include "../materials/material.h"
#include "../utilities/types.h"
#include "ray.h"

class hit_record
{
public:
	point3 hit_point{}; // point (coordinates) where the hit occurred
	vector3 normal{}; // normal vector where the hit occurred
	std::shared_ptr<material> mat = nullptr; // material of the object hit by the ray
	
	/// <summary>
	/// The t value in the hit_record specifies the distance from the ray origin A to the intersection point along the direction B.
	/// When a ray intersects an object, the t value is used to compute the exact position of the hit point :
	/// </summary>
	double t = 0.0; // scalar parameter that moves the point along the ray.


	double u = 0.0; // u mapping coordinate
	double v = 0.0; // v mapping coordinate
	bool front_face = true; // front-face tracking (object was hit from outside (frontface) or inside (backface) ?)
	std::string name; // name of the object that was hit
	aabb bbox; // bounding box size of the object that was hit
	
	vector3 tangent{}; // tangent vector calculated from the normal (obj models only)
	vector3 bitangent{}; // bitangent vector calculated from the normal (obj models only)

	void set_face_normal(const ray& r, const vector3& outward_normal);
};