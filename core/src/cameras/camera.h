#pragma once

#include "../misc/ray.h"
#include "../misc/scene.h"
#include "../misc/color.h"
#include "../renderParameters.h"

class camera
{
public:
	double  aspect_ratio = 1.0;             // Ratio of image width over height
	int     image_width = 400;              // Rendered image width in pixel count
	int     samples_per_pixel = 10;         // Count of random samples for each pixel (antialiasing)
	int     max_depth = 10;                 // Maximum number of ray bounces into scene
	
	
	virtual ~camera() = default;

	virtual void initialize(const renderParameters& params) = 0;

	virtual const ray get_ray(int i, int j, int s_i, int s_j) const = 0;
	virtual color ray_color(const ray& r, int depth, scene& _scene) = 0;

	const int getImageHeight() const;
	const int getImageWidth() const;
	const int getSqrtSpp() const;
	const int getMaxDepth() const;
	const int getSamplePerPixel() const;

protected:
	int    image_height = 0;    // Rendered image height
	int    sqrt_spp;        // Square root of number of samples per pixel
	double recip_sqrt_spp;  // 1 / sqrt_spp
};