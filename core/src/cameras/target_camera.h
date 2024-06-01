#pragma once

//#include "../constants.h"
#include "../misc/color.h"
#include "../misc/ray.h"
#include "../utilities/types.h"
#include "../utilities/randomizer.h"
#include "../misc/renderParameters.h"

#include "camera.h"
#include "../misc/scene.h"

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
    const ray get_ray(int i, int j, int s_i, int s_j) const override;

    /// <summary>
    /// Fire a given ray and get the hit record (recursive)
    /// </summary>
    /// <param name="r"></param>
    /// <param name="world"></param>
    /// <returns></returns>
    color ray_color(const ray& r, int depth, scene& _scene, randomizer& random) override;

    /// <summary>
    /// Returns a random point in the square surrounding a pixel at the origin, given the two subpixel indices (usefull for antialiasing)
    /// </summary>
    /// <returns></returns>
    vector3 pixel_sample_square(int s_i, int s_j) const;


private:
    point3 center;          // Camera center
    point3 pixel00_loc;     // Location of pixel 0, 0
    vector3   pixel_delta_u;   // Offset to pixel to the right
    vector3   pixel_delta_v;   // Offset to pixel below
    vector3   u, v, w;         // Camera frame basis vectors
    vector3   defocus_disk_u;  // Defocus disk horizontal radius
    vector3   defocus_disk_v;  // Defocus disk vertical radius


    point3 defocus_disk_sample() const;

    vector3 direction_from(const point3& light_pos, const point3& hit_point) const;

    //color calculate_spotlight_contribution(const hit_record& rec, scene& _scene);
};