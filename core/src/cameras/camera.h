#pragma once

#include "../misc/ray.h"

class camera
{
public:
	virtual ~camera() = default;

	virtual const ray get_ray(int i, int j, int s_i, int s_j) const = 0;
};