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

/// <summary>
/// https://github.com/kamiyo/RayTra/blob/master/RayTra/Torus.cpp
/// </summary>
class torus : public hittable
{
public:
	torus(std::string _name = "Torus");
	torus(vector3 center, float _majorRadius, float _minorRadius, std::shared_ptr<material> _material, std::string _name = "Torus");


	/// <summary>
	/// Logic of sphere ray hit detection
	/// </summary>
	/// <param name="r"></param>
	/// <param name="ray_t"></param>
	/// <param name="rec"></param>
	/// <returns></returns>
	virtual bool hit(const ray& r, interval ray_t, hit_record& rec, int depth) const override;

	virtual aabb bounding_box() const override;

private:
	vector3 center;
	double majorRadius;
	double minorRadius;
	std::shared_ptr<material> mat;
	double _R2, _R2r2;
	aabb bbox; // bounding box

	void get_torus_uv(const vector3& _p, double& _u, double& _v, double _majorRadius, double _minorRadius) const;

	/// <summary>
	/// Update the internal AABB of the mesh.
	/// Warning: run this when the mesh is updated.
	/// </summary>
	void updateBoundingBox() override;
};