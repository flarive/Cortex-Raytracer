#pragma once

#include "../misc/ray.h"
#include "../misc/color.h"
#include "../materials/material.h"
#include "../primitives/hittable_list.h"
#include "../misc/scatter_record.h"
#include "../utilities/randomizer.h"

/// <summary>
/// Phong material
/// https://stackoverflow.com/questions/24132774/trouble-with-phong-shading
/// </summary>
class phong2 : public material
{
public:

    phong2(std::shared_ptr<texture> diffuseTexture, std::shared_ptr<texture> specularTexture, const color& ambientColor, double shininess);

    phong2(std::shared_ptr<texture> diffuseTexture, std::shared_ptr<texture> specularTexture, std::shared_ptr<texture> bumpTexture, std::shared_ptr<texture> normalTexture, const color& ambientColor, double shininess);

    bool scatter(const ray& r_in, const hittable_list& lights, const hit_record& rec, scatter_record& srec, randomizer& random) const override;
    double scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered) const override;

private:
    color m_ambientColor{};
    double m_shininess = 0.0;

    static double maxDot3(const vector3& v1, const vector3& v2);
};

