//#pragma once
//
//#include "hittable.h"
//#include "../misc/ray.h"
//#include "../misc/hit_record.h"
//#include "../utilities/types.h"
//#include "../utilities/interval.h"
//#include "../materials/material.h"
//#include "../aabb.h"
//
//class mesh : public hittable
//{
//public:
//	mesh();
//	mesh(std::string filepath, std::shared_ptr<material> mat, bool use_mtl, bool use_smoothing);
//
//	virtual bool hit(const ray& r, interval ray_t, hit_record& rec, int depth) const override;
//	virtual aabb bounding_box() const override;
//
//};