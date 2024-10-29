#include "random_sampler.h"

#include "../misc/singleton.h"


random_sampler::random_sampler(const vector3& pixel_delta_u, const vector3& pixel_delta_v, int samples)
    : sampler(pixel_delta_u, pixel_delta_v, samples)
{
}

/// <summary>
/// Simple random sampler Anti-Aliasing
/// A stochastic sample pattern is a random distribution of multisamples throughout the pixel.
/// The irregular spacing of samples makes attribute evaluation complicated.
/// The method is cost efficient due to low sample count (compared to regular grid patterns).
/// Edge optimization with this method, although sub-optimal for screen aligned edges.
/// Image quality is excellent for a moderate number of samples.
/// </summary>
/// <param name="s_i"></param>
/// <param name="s_j"></param>
/// <returns></returns>
vector3 random_sampler::generate_samples(int s_i, int s_j, randomizer& rnd) const
{
    // Generate random positions within the pixel
    auto px = -0.5 + m_recip_sqrt_spp * (s_i + rnd.get_real(0.0, 1.0));
    auto py = -0.5 + m_recip_sqrt_spp * (s_j + rnd.get_real(0.0, 1.0));
    return (px * m_pixel_delta_u) + (py * m_pixel_delta_v);
}