#pragma once

#include "../utilities/types.h"
#include "texture.h"
#include "../misc/color.h"

class roughness_from_sharpness_texture : public texture
{
public:
    roughness_from_sharpness_texture();

    roughness_from_sharpness_texture(std::shared_ptr<texture> sharpness_map, double min_v, double max_v);

    color value(double u, double v, const point3& p) const override;


public:
    std::shared_ptr<texture> sharpness_text;
private:
    double l_min_val, l_max_val;
};