#pragma once

#include "texture.h"
#include "../misc/color.h"
#include "../utilities/types.h"

/// <summary>
/// Emissive texture
/// </summary>
class emissive_texture : public texture
{
public:
	emissive_texture();
	emissive_texture(std::shared_ptr<texture> bump, double strength = 10.0);

	color value(double u, double v, const point3& p) const;

private:
	std::shared_ptr<texture> m_emissive = nullptr;
	double m_strength = 10.0;     // Scaling factor for emissive effect
};