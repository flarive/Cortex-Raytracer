#pragma once

#include "../utilities/types.h"
#include "texture.h"
#include "../misc/color.h"

/// <summary>
/// Displacement texture
/// https://stackoverflow.com/questions/4476669/ray-tracing-a-sphere-with-displacement-mapping
/// </summary>
class displacement_texture : public texture
{
public:
    displacement_texture();

    displacement_texture(std::shared_ptr<texture> t, double strength);

    color value(double u, double v, const point3& p) const override;

    double getStrenth();

    float getDisplacement(float u, float v) const;

private:
    std::shared_ptr<texture> m_displacement = nullptr;
    float m_strength = 10.0;

    int m_width = 0;
    int m_height = 0;
    int m_channels = 3;

    float* m_data = nullptr;
};