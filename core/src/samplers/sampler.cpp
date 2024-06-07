#include "sampler.h"


sampler::sampler(const vector3& pixel_delta_u, const vector3& pixel_delta_v, int samples) : m_spp(samples)
{
}

vector3 sampler::generate_samples(int s_i, int s_j) const
{
	return vector3{};
}