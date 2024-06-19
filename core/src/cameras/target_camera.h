#pragma once

#include "../misc/scene.h"
#include "../misc/color.h"
#include "../misc/ray.h"
#include "../utilities/types.h"
#include "../utilities/randomizer.h"
#include "../misc/renderParameters.h"


#include "camera.h"


class target_camera : public camera
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
    const ray get_ray(int i, int j, int s_i, int s_j, std::shared_ptr<sampler> aa_sampler) const override;

    /// <summary>
    /// Fire a given ray and get the hit record (recursive)
    /// </summary>
    /// <param name="r"></param>
    /// <param name="world"></param>
    /// <returns></returns>
    color ray_color(const ray& r, int depth, scene& _scene, randomizer& random) override;

private:
    point3 defocus_disk_sample() const;

    vector3 direction_from(const point3& light_pos, const point3& hit_point) const;

    color get_background_image_color(int x, int y, const vector3& unit_dir, std::shared_ptr<image_texture> background_texture, bool background_iskybox);
};