#include "msaa_sampler.h"

#include "../utilities/randomizer.h"

msaa_sampler::msaa_sampler(const vector3& pixel_delta_u, const vector3& pixel_delta_v, int samples)
    : sampler(pixel_delta_u, pixel_delta_v, samples)
{
}

/// <summary>
/// Multisample Anti-Aliasing (MSAA) 
/// </summary>
/// <param name="s_i"></param>
/// <param name="s_j"></param>
/// <returns></returns>
vector3 msaa_sampler::generate_samples(int s_i, int s_j) const
{
    // Typically, MSAA would require edge detection. Here, we perform a basic multisampling.
    // Instead of random jitter, we use a predefined pattern.
    // MSAA predefined sample positions
    constexpr int num_samples = 8; // Number of samples per pixel
    const double sample_offsets[num_samples][2] = {
        {-0.375, -0.375},
        {-0.125, -0.375},
        {0.125, -0.375},
        {0.375, -0.375},
        {-0.375, 0.375},
        {-0.125, 0.375},
        {0.125, 0.375},
        {0.375, 0.375}
    };

    // Calculate the sample position based on the predefined sample offsets
    auto px = -0.5 + (s_i + sample_offsets[num_samples][0]) * m_recip_sqrt_spp;
    auto py = -0.5 + (s_j + sample_offsets[num_samples][1]) * m_recip_sqrt_spp;

    return (px * m_pixel_delta_u) + (py * m_pixel_delta_v);
}
