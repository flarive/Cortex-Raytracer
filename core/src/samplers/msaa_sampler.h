#pragma once

#include "sampler.h"
#include "../utilities/randomizer.h"

class msaa_sampler : public sampler
{
public:
	msaa_sampler(const vector3& pixel_delta_u, const vector3& pixel_delta_v, int samples = 50, int spp = 8);

	virtual vector3 generate_samples(int s_i, int s_j) const override;

private:
	vector3 pixel_sample_msaa(int s_i, int s_j, int num_samples, const double sample_offsets[][2]) const;
};