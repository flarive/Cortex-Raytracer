#pragma once

#include "sampler.h"

class msaa_sampler : public sampler
{
public:
	msaa_sampler(const vector3& pixel_delta_u, const vector3& pixel_delta_v, int samples = 50);

	virtual vector3 generate_samples(int s_i, int s_j) const override;
};