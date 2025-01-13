#pragma once

#include "../misc/ray.h"
#include "../misc/color.h"
#include "../materials/material.h"
#include "../primitives/hittable_list.h"
#include "../misc/scatter_record.h"
#include "../randomizers/randomizer.h"

/// <summary>
/// Phong material
/// https://stackoverflow.com/questions/24132774/trouble-with-phong-shading
///
/// Ambient lighting: even when it is dark there is usually still some light somewhere in the world
/// (the moon, a distant light) so objects are almost never completely dark.
/// To simulate this we use an ambient lighting constant that always gives the object some color.
///
/// Diffuse lighting : simulates the directional impact a light object has on an object.
/// This is the most visually significant component of the lighting model.
/// The more a part of an object faces the light source, the brighter it becomes.
///
/// Specular lighting : simulates the bright spot of a light that appears on shiny objects.
/// Specular highlights are more inclined to the color of the light than the color of the object.
/// </summary>
class phong_material : public material
{
public:

    phong_material(std::shared_ptr<texture> diffuseTexture);
    
    phong_material(std::shared_ptr<texture> diffuseTexture, std::shared_ptr<texture> specularTexture, const color& ambientColor, double shininess);

    phong_material(std::shared_ptr<texture> diffuseTexture, std::shared_ptr<texture> specularTexture, std::shared_ptr<texture> bumpTexture, std::shared_ptr<texture> normalTexture, std::shared_ptr<texture> displaceTexture, std::shared_ptr<texture> alphaTexture, std::shared_ptr<texture> emissiveTexture, const color& ambientColor, double shininess);

    bool scatter(const ray& r_in, const hittable_list& lights, const hit_record& rec, scatter_record& srec, randomizer& rnd) const override;
    double scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered) const override;


private:
    color m_ambientColor{};
    double m_shininess = 0.0;
};