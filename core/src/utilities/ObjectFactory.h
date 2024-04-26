#pragma once

#include "../primitives/hittable.h"
#include "../materials/material.h"
#include "types.h"


class ObjectFactory
{
public:
	ObjectFactory() = delete;

	static std::shared_ptr<hittable> createBox(const point3& p0, const point3& p1, const std::shared_ptr<material>& material);

	static std::shared_ptr<hittable> createCylinder(const point3& center, double radius, double height, const std::shared_ptr<material>& material);

	static std::shared_ptr<hittable> createSphere(const point3& center, double radius, const std::shared_ptr<material>& material);

	static std::shared_ptr<hittable> createCone(const point3& center, double height, double radius, const std::shared_ptr<material>& material);

	static std::shared_ptr<hittable> createPlane(const point3& p0, point3 p1, const std::shared_ptr<material>& material);
};