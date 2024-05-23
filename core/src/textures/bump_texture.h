#pragma once

#include "texture.h"
#include "../misc/color.h"
#include "../utilities/types.h"

/// <summary>
/// Bump texture
/// </summary>
class bump_texture : public texture
{
public:
	bump_texture();
	bump_texture(std::shared_ptr<texture> bump, double scale);

	color value(double u, double v, const point3& p) const;

	vector3 perturb_normal(const vector3& normal, float u, float v, const vector3& p) const;
	vector3 convert_to_world_space(const vector3& tangentSpaceNormal, const vector3& originalNormal) const;
	void compute_tangent_space(const vector3& normal, vector3& tangent, vector3& bitangent) const;

private:
	std::shared_ptr<texture> m_bump = nullptr;
	double m_scale = 0.0;     // Scaling factor for bump effect
};