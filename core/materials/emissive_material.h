#pragma once

#include "../constants.h"
#include "../misc/ray.h"
#include "../misc/color.h"
#include "../textures/texture.h"
#include "../textures/solid_color_texture.h"
#include "../primitives/hittable.h"
#include "../materials/material.h"
#include "../primitives/hittable_list.h"
#include "../misc/scatter_record.h"
#include "../randomizers/randomizer.h"


class emissive_material : public material
{
public:
    emissive_material(std::shared_ptr<texture> a);
    emissive_material(std::shared_ptr<texture> a, double _intensity);


    emissive_material(color _c);
    emissive_material(color _c, double _intensity);

    color emitted(const ray& r_in, const hit_record& rec, double u, double v, const point3& p) const override;


private:
    std::shared_ptr<texture> m_emit = nullptr;
    double m_intensity = 1.0;
    bool m_directional = true;
    bool m_invisible = true;
};

