#pragma once

#include "aabb.h"
#include "primitives/box.h"


class aabb_debug
{
public:
	static std::shared_ptr<box> aabb_to_box_primitive(const aabb& box);
};