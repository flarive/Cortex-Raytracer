#pragma once

#include "sampler.h"

class ssaa_sampler : public sampler
{
public:
	ssaa_sampler(const vector3& pixel_delta_u, const vector3& pixel_delta_v, int samples = 50);

	virtual vector3 generate_samples(int s_i, int s_j) const override;
};

