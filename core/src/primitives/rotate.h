#pragma once

#include "hittable.h"
#include "../misc/ray.h"
#include "../utilities/interval.h"
#include "../materials/material.h"
#include "../aabb.h"
#include "../misc/hit_record.h"

namespace myOwnRaytracer
{
	class rotate : public hittable
	{
	public:
		rotate(std::shared_ptr<hittable> _p, double _angle, int _axis);
		bool hit(const ray& r, interval ray_t, hit_record& rec, int depth) const override;
		aabb bounding_box() const override;

	private:
		std::shared_ptr<hittable> object;
		double sin_theta, cos_theta;
		//bool hasbox;
		int axis;
		aabb bbox;

		/// <summary>
		/// Update the internal AABB of the mesh.
		/// Warning: run this when the mesh is updated.
		/// </summary>
		void updateBoundingBox() override;
	};
}