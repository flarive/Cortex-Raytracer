#pragma once

#include "../misc/ray.h"
#include "../misc/color.h"
#include "../textures/texture.h"
#include "../materials/material.h"
#include "../misc/hit_record.h"
#include "../utilities/math_utils.h"

/// <summary>
/// Diffuse spot light emissive material
/// https://github.com/tylermorganwall/rayrender/blob/1feb2f1328c9eab5d67c09a53ffea6d8d403c064/src/material.h#L167
/// </summary>
class diffuse_spot_light : public material
{
public:
    diffuse_spot_light(std::shared_ptr<texture> emitTex, point3 pos, vector3 dir, double cutoff, double falloff, double blur, double intensity, bool invisible);

    color emitted(const ray& r_in, const hit_record& rec, double u, double v, const point3& p) const override;

private:
    std::shared_ptr<texture> m_emit = nullptr;
    point3 m_position{};
    double m_intensity = 1.0;
    bool m_invisible = true;
    bool m_directional = true;
    vector3 m_direction = vector3(0, 0, -1); // Default direction
    double m_cutoff = cos(degrees_to_radians(30.0)); // Cutoff angle in radians
    double m_falloff = 1.0; // Falloff exponent
    double m_blur = 1.0;
};