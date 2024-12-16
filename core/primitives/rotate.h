#pragma once

#include "hittable.h"
#include "../misc/ray.h"
#include "../utilities/interval.h"
#include "../misc/aabb.h"
#include "../misc/hit_record.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace rt
{
	/// <summary>
	/// https://github.com/njeff/raytracer0/blob/27de69303cd25c77b126391fdd4c7c24b4ff3de7/week/Hittable.java#L67
	/// </summary>
	class rotate : public hittable
	{
	public:
		rotate(std::shared_ptr<hittable> _p, const vector3& _rotation);
		bool hit(const ray& r, interval ray_t, hit_record& rec, int depth, randomizer& rnd) const override;
		aabb bounding_box() const override;

	private:
		std::shared_ptr<hittable> m_object;
		double sin_theta = 0;
		double cos_theta = 0;
		aabb bbox;

		vector3 m_rotation{};

		/// <summary>
		/// Update the internal AABB of the mesh.
		/// Warning: run this when the mesh is updated.
		/// </summary>
		void updateBoundingBox() override;
	};
}