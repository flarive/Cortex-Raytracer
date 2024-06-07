#include "ssaa_sampler.h"

#include "../utilities/randomizer.h"

ssaa_sampler::ssaa_sampler(const vector3& pixel_delta_u, const vector3& pixel_delta_v, int samples)
    : sampler(pixel_delta_u, pixel_delta_v, samples)
{
}

/// <summary>
/// Anti-Aliasing (SSAA), specifically using a technique called stochastic supersampling or jittered samplingg
/// </summary>
/// <param name="s_i"></param>
/// <param name="s_j"></param>
/// <returns></returns>
vector3 ssaa_sampler::generate_samples(int s_i, int s_j) const
{
    auto px = -0.5 + m_recip_sqrt_spp * (s_i + randomizer::random_double());
    auto py = -0.5 + m_recip_sqrt_spp * (s_j + randomizer::random_double());
    return (px * m_pixel_delta_u) + (py * m_pixel_delta_v);
}