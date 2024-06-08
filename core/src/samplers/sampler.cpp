#include "sampler.h"


sampler::sampler(const vector3& pixel_delta_u, const vector3& pixel_delta_v, int samples) 
	: m_pixel_delta_u(pixel_delta_u), m_pixel_delta_v(pixel_delta_v), m_spp(samples)
{
	int sqrt_spp = static_cast<int>(sqrt(samples));
	m_recip_sqrt_spp = 1.0 / sqrt_spp;
}

vector3 sampler::generate_samples(int s_i, int s_j) const
{
	return vector3{};
}