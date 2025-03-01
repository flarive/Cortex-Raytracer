#pragma once

#include "../constants.h"
#include "hittable.h"
#include "hittable_list.h"
#include "../misc/ray.h"
#include "../misc/aabb.h"
#include "../utilities/interval.h"
#include "../misc/hit_record.h"
#include "../utilities/math_utils.h"
#include "../utilities/uvmapping.h"

#include <cmath>

/// <summary>
/// https://github.com/kamiyo/RayTra/blob/master/RayTra/Torus.cpp
/// </summary>
class torus : public hittable
{
public:
	torus(std::string _name = "Torus");
	torus(point3 _center, float _majorRadius, float _minorRadius, std::shared_ptr<material> _material, std::string _name = "Torus");
	torus(point3 _center, float _majorRadius, float _minorRadius, std::shared_ptr<material> _material, const uvmapping& _mapping, std::string _name = "Torus");

	/// <summary>
	/// Logic of sphere ray hit detection
	/// </summary>
	/// <param name="r"></param>
	/// <param name="ray_t"></param>
	/// <param name="rec"></param>
	/// <returns></returns>
	virtual bool hit(const ray& r, interval ray_t, hit_record& rec, int depth, randomizer& rnd) const override;

	virtual aabb bounding_box() const override;

private:
	point3 center;
	double majorRadius = 0;
	double minorRadius = 0;
	std::shared_ptr<material> mat;
	double _R2 = 0, _R2r2 = 0;

	/// <summary>
	/// Update the internal AABB of the mesh.
	/// Warning: run this when the mesh is updated.
	/// </summary>
	void updateBoundingBox() override;
};