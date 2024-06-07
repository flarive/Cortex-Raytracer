#pragma once

#include "../misc/ray.h"
#include "../misc/scene.h"
#include "../misc/color.h"
#include "../pdf/pdf.h"
#include "../textures/image_texture.h"
#include "../utilities/types.h"
#include "../utilities/randomizer.h"
#include "../misc/renderParameters.h"
#include "../samplers/sampler.h"

class camera
{
public:
	double  aspect_ratio = 1.0;             // Ratio of image width over height
	int     image_width = 400;              // Rendered image width in pixel count
	int     samples_per_pixel = 10;         // Count of random samples for each pixel (antialiasing)
	int     max_depth = 10;                 // Maximum number of ray bounces into scene

	double  vfov = 90;                      // Vertical view angle (field of view) (90 is for wide-angle view for example)
	point3  lookfrom = point3(0, 0, -1);    // Point camera is looking from
	point3  lookat = point3(0, 0, 0);       // Point camera is looking at
	vector3 vup = vector3(0, 1, 0);            // Camera-relative "up" direction

	// Depth of field
	double  defocus_angle = 0;              // Variation angle of rays through each pixel
	double  focus_dist = 10;                // Distance from camera lookfrom point to plane of perfect focus

	color   background_color;               // Scene background color
	std::shared_ptr<image_texture> background_texture;
	std::shared_ptr<pdf> background_pdf;
	bool background_iskybox;

	
	camera();
	virtual ~camera() = default;

	virtual void initialize(const renderParameters& params) = 0;

	virtual const ray get_ray(int i, int j, int s_i, int s_j, Sampler& sampler) const = 0;
	virtual color ray_color(const ray& r, int depth, scene& _scene, randomizer& random) = 0;

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