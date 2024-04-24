#pragma once

#include "../misc/ray.h"
#include "../misc/color.h"
#include "../textures/texture.h"
#include "../materials/material.h"
#include "../misc/scatter_record.h"


// See https://people.sc.fsu.edu/~jburkardt/data/mtl/mtl.html
// https://paulbourke.net/dataformats/mtl/
// The MTL format is based on the Phong shading model, so this uses a bit of reinterpretation
// See https://www.scratchapixel.com/lessons/3d-basic-rendering/phong-shader-BRDF , and https://www.psychopy.org/api/visual/phongmaterial.html , and http://vr.cs.uiuc.edu/node198.html
// There are a few properties, which we allow to vary based on textures: 
// diffuse color: albedo for lambertian 
// specular color: albedo for metal
// emissive color: emissive :)
//
// sharpness map: remapped to fuzz := 1-log_10(sharpness)/4, sharpness clamped to [1, 10000]
//
// How to decide what happens? |color_for_type| / (sum ^type |color|), i.e. if color components add to 1, everything is fine, if not it is normalized
//
class mtl_material : public material
{
public:
    mtl_material(
        std::shared_ptr<texture> diffuse_a,
        std::shared_ptr<texture> specular_a,
        std::shared_ptr<texture> emissive_a,
        std::shared_ptr<texture> transparency_map,
        std::shared_ptr<texture> sharpness_map,
        int illum);

    bool scatter(const ray& r_in, const hittable_list& lights, const hit_record& rec, scatter_record& srec) const override;

    color emitted(const ray& r_in, const hit_record& rec, double u, double v, const point3& p) const override;

    double scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered) const override;



public:
    std::shared_ptr<texture> emissive_text;
    std::shared_ptr<texture> diffuse_text;
    std::shared_ptr<texture> specular_text;
    std::shared_ptr<texture> transparency_text;
    std::shared_ptr<texture> roughness_text;
private:
    std::shared_ptr<material> emissive_mat;
    std::shared_ptr<material> diffuse_mat;
    std::shared_ptr<material> specular_mat;

    inline double transparency_prob(double u, double v, const point3& p) const;
    inline double diffuse_prob(double u, double v, const point3& p) const;
    inline std::shared_ptr<material> choose_mat(double u, double v, const point3& p) const;
};