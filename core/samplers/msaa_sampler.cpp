#include "msaa_sampler.h"

msaa_sampler::msaa_sampler(const vector3& pixel_delta_u, const vector3& pixel_delta_v, int samples, int spp)
    : sampler(pixel_delta_u, pixel_delta_v, samples, spp)
{
}

/// <summary>
/// Multisample Anti-Aliasing (MSAA) 
/// </summary>
/// <param name="s_i"></param>
/// <param name="s_j"></param>
/// <returns></returns>
vector3 msaa_sampler::generate_samples(int s_i, int s_j, randomizer& rnd) const
{
    // Typically, MSAA would require edge detection. Here, we perform a basic multisampling.
    // Instead of random jitter, we use a predefined pattern.
    // MSAA predefined sample positions

    vector3 color_sum(0, 0, 0);

    if (m_spp >= 8)
    {
        // 5mn23s
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

        for (int i = 0; i < num_samples; ++i)
        {
            color_sum += pixel_sample_msaa(s_i, s_j, i, sample_offsets);
        }
    }
    else if (m_spp >= 4)
    {
        constexpr int num_samples = 4; // Number of samples per pixel
        const double sample_offsets[num_samples][2] = {
            {-0.25, -0.25},
            {0.25, -0.25},
            {-0.25, 0.25},
            {0.25, 0.25}
        };

        for (int i = 0; i < num_samples; ++i)
        {
            color_sum += pixel_sample_msaa(s_i, s_j, i, sample_offsets);
        }
    }
    else if (m_spp >= 2)
    {
        constexpr int num_samples = 2; // Number of samples per pixel
        const double sample_offsets[num_samples][2] = {
            {-0.25, -0.25},
            {0.25, 0.25}
        };

        for (int i = 0; i < num_samples; ++i)
        {
            color_sum += pixel_sample_msaa(s_i, s_j, i, sample_offsets);
        }
    }
    
    return color_sum / static_cast<double>(m_spp);
}

vector3 msaa_sampler::pixel_sample_msaa(int s_i, int s_j, int sample_index, const double sample_offsets[][2]) const
{
    // Ensure sample_index is within bounds
    if (sample_index < 0 || sample_index >= m_spp)
        sample_index = 0; // Default to first sample if index is out of bounds

    // Calculate the sample position based on the predefined sample offsets
    auto px = -0.5 + (s_i + sample_offsets[sample_index][0]) * m_recip_sqrt_spp;
    auto py = -0.5 + (s_j + sample_offsets[sample_index][1]) * m_recip_sqrt_spp;

    return (px * m_pixel_delta_u) + (py * m_pixel_delta_v);
}