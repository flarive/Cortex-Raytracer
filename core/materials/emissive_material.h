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
    emissive_material(std::shared_ptr<texture> emissiveTex, double _intensity);


    emissive_material(color _c);
    emissive_material(color _c, double _intensity);

    bool scatter(const ray& r_in, const hittable_list& lights, const hit_record& rec, scatter_record& srec, randomizer& rnd) const override;
    double scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered) const override;

    color emitted(const ray& r_in, const hit_record& rec, double u, double v, const point3& p) const override;


private:
    std::shared_ptr<texture> m_emissive = nullptr;
    double m_intensity = 1.0;
    //bool m_directional = true;
    //bool m_invisible = true;
};

