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