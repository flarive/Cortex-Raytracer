#pragma once

#include "../utilities/types.h"

/// <summary>
/// Sampler base class
/// https://cgg.mff.cuni.cz/~pepca/lectures/pdf/pg2-13-sampling.en.pdf
/// </summary>
class sampler
{
public:
    sampler(const vector3& pixel_delta_u, const vector3& pixel_delta_v, int samples = 50);

    virtual vector3 generate_samples(int s_i, int s_j) const;
                        

protected:
    int m_spp = 0;
    double m_recip_sqrt_spp = 0;

    vector3 m_pixel_delta_u{};
    vector3 m_pixel_delta_v{};
};
