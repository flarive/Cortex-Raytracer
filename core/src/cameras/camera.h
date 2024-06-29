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

	color   background_color{};               // Scene background color
	std::shared_ptr<image_texture> background_texture = nullptr;
	std::shared_ptr<pdf> background_pdf = nullptr;
	bool background_iskybox = false;

	bool is_orthographic = false;
	double ortho_height = 0.0;

	
	camera();
	virtual ~camera() = default;

	virtual void initialize(const renderParameters& params) = 0;

	/// <summary>
	/// Fire a given ray and get the hit record (recursive)
	/// </summary>
	/// <param name="r"></param>
	/// <param name="world"></param>
	/// <returns></returns>
	virtual const ray get_ray(int i, int j, int s_i, int s_j, std::shared_ptr<sampler> aa_sampler) const = 0;

	/// <summary>
	/// Calculate ray color
	/// </summary>
	/// <param name="r"></param>
	/// <param name="depth"></param>
	/// <param name="_scene"></param>
	/// <param name="random"></param>
	/// <returns></returns>
	virtual color ray_color(const ray& r, int depth, scene& _scene, randomizer& random);

	const int getImageHeight() const;
	const int getImageWidth() const;
	const int getSqrtSpp() const;
	const int getMaxDepth() const;
	const int getSamplePerPixel() const;

	const vector3 get_pixel_delta_u() const;
	const vector3 get_pixel_delta_v() const;




protected:
	int			image_height = 0;    // Rendered image height
	int			sqrt_spp = 0;        // Square root of number of samples per pixel
	double		recip_sqrt_spp = 0.0;  // 1 / sqrt_spp

	point3      center{};          // Camera center
	point3      pixel00_loc{};     // Location of pixel 0, 0
	vector3     pixel_delta_u{};   // Offset to pixel to the right
	vector3     pixel_delta_v{};   // Offset to pixel below
	vector3     u{}, v{}, w{};     // Camera frame basis vectors
	vector3     defocus_disk_u{};  // Defocus disk horizontal radius
	vector3     defocus_disk_v{};  // Defocus disk vertical radius

	

	point3 defocus_disk_sample() const;

	vector3 direction_from(const point3& light_pos, const point3& hit_point) const;

	color get_background_image_color(int x, int y, const vector3& unit_dir, std::shared_ptr<image_texture> background_texture, bool background_iskybox);
};