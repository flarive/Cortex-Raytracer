#pragma once

#include "../misc/ray.h"
#include "../misc/color.h"
#include "../materials/material.h"
#include "../primitives/hittable_list.h"
#include "../misc/scatter_record.h"
#include "../randomizers/randomize.h"

/// <summary>
/// Phong material
/// https://stackoverflow.com/questions/24132774/trouble-with-phong-shading
/// </summary>
class phong : public material
{
public:

    phong(std::shared_ptr<texture> diffuseTexture, std::shared_ptr<texture> specularTexture, const color& ambientColor, double shininess);

    phong(std::shared_ptr<texture> diffuseTexture, std::shared_ptr<texture> specularTexture, std::shared_ptr<texture> bumpTexture, std::shared_ptr<texture> normalTexture, std::shared_ptr<texture> displaceTexture, std::shared_ptr<texture> alphaTexture, std::shared_ptr<texture> emissiveTexture, const color& ambientColor, double shininess);

    bool scatter(const ray& r_in, const hittable_list& lights, const hit_record& rec, scatter_record& srec, randomizer& rnd) const override;
    double scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered) const override;


private:
    color m_ambientColor{};
    double m_shininess = 0.0;
};