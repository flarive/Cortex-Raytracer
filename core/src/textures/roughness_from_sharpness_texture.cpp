#include "roughness_from_sharpness_texture.h"

#include "../misc/color.h"

roughness_from_sharpness_texture::roughness_from_sharpness_texture()
{
}

roughness_from_sharpness_texture::roughness_from_sharpness_texture(std::shared_ptr<texture> sharpness_map, double min_v, double max_v)
	: sharpness_text(sharpness_map), l_min_val(log(min_v)), l_max_val(log(max_v))
{
}

color roughness_from_sharpness_texture::value(double u, double v, const point3& p) const
{
	return color(1, 0, 0) * std::clamp(
		log(sharpness_text->value(u, v, p).length() + 0.00001), l_min_val, l_max_val)
		/ (l_max_val - l_min_val);
}