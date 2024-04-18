#pragma once

#include "hittable.h"
#include "../misc/ray.h"
#include "../utilities/interval.h"
#include "../aabb.h"
#include "../misc/hit_record.h"

namespace raytracer
{
	/// <summary>
	/// https://github.com/njeff/raytracer0/blob/27de69303cd25c77b126391fdd4c7c24b4ff3de7/week/Hittable.java#L67
	/// </summary>
	class rotate : public hittable
	{
	public:
		rotate(std::shared_ptr<hittable> _p, double _angle, int _axis);
		bool hit(const ray& r, interval ray_t, hit_record& rec, int depth) const override;
		aabb bounding_box() const override;

	private:
		std::shared_ptr<hittable> object;
		double sin_theta = 0;
		double cos_theta = 0;
		int axis = 0;

		/// <summary>
		/// Update the internal AABB of the mesh.
		/// Warning: run this when the mesh is updated.
		/// </summary>
		void updateBoundingBox() override;
	};
}