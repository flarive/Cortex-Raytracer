#pragma once

#include "../misc/scene.h"
#include "../misc/color.h"
#include "../misc/ray.h"
#include "../utilities/types.h"
#include "../misc/renderParameters.h"

#include "camera.h"

/// <summary>
/// Orthographic camera with target
/// </summary>
class orthographic_camera : public camera
{
public:
    /// <summary>
    /// Initialize camera with settings
    /// </summary>
    /// <param name="params"></param>
    void initialize(const renderParameters& params) override;

    /// <summary>
    /// Get a randomly-sampled camera ray for the pixel at location i,j, originating from the camera defocus disk,
    /// and randomly sampled around the pixel location
    /// </summary>
    /// <param name="i"></param>
    /// <param name="j"></param>
    /// <returns></returns>
    const ray get_ray(int i, int j, int s_i, int s_j, std::shared_ptr<sampler> aa_sampler, randomizer& rnd) const override;

private:
};