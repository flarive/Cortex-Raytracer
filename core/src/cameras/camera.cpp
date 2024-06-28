#include "camera.h"

camera::camera()
{
}

const int camera::getImageHeight() const
{
	return image_height;
}

const int camera::getImageWidth() const
{
	return image_width;
}

const int camera::getSqrtSpp() const
{
	return sqrt_spp;
}

const int camera::getMaxDepth() const
{
	return max_depth;
}

const int camera::getSamplePerPixel() const
{
	return samples_per_pixel;
}

const vector3 camera::get_pixel_delta_u() const
{
	return pixel_delta_u;
}

const vector3 camera::get_pixel_delta_v() const
{
	return pixel_delta_v;
}


point3 camera::camera::defocus_disk_sample() const
{
	// Returns a random point in the camera defocus disk.
	auto p = randomizer::random_in_unit_disk();
	return center + (p.x * defocus_disk_u) + (p.y * defocus_disk_v);
}


vector3 camera::direction_from(const point3& light_pos, const point3& hit_point) const
{
	// Calculate the direction from the hit point to the light source.
	return randomizer::unit_vector(light_pos - hit_point);
}

color camera::get_background_image_color(int x, int y, const vector3& unit_dir, std::shared_ptr<image_texture> background_texture, bool background_iskybox)
{
	double u, v;

	if (background_iskybox)
		get_spherical_uv(unit_dir, u, v);
	else
		get_screen_uv(x, y, background_texture->getWidth(), background_texture->getHeight(), getImageWidth(), getImageHeight(), u, v);

	return background_texture->value(u, v, unit_dir);
}