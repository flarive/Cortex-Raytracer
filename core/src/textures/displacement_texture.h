#pragma once

#include "../utilities/types.h"
#include "texture.h"
#include "../misc/color.h"
//#include "../utilities/bitmap_image.h"

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

    // Modify the hit point based on the displacement map
    //point3 displace(const point3& point) const;

    vector3 displace_point(const vector3& p, double displacement, const vector3& normal) const;
    vector3 normal_function(const vector3& p) const;

private:
    std::shared_ptr<texture> m_displacement = nullptr;
    double m_strength = 10.0;

    double distance_function(const vector3& p) const;
};