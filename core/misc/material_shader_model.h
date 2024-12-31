#pragma once

#include "../materials/material.h"
#include "../materials/lambertian_material.h"
#include "../materials/phong_material.h"
#include "../materials/anisotropic_material.h"
#include "../materials/isotropic_material.h"
#include "../materials/metal_material.h"
#include "../materials/dielectric_material.h"
#include "../materials/oren_nayar_material.h"
#include "../materials/emissive_material.h"

#include <memory>
#include <ostream>

enum class material_shader_model
{
    Undefined = 0,
    Lambertian = 1,
    Phong = 2,
    Anisotropic = 3,
    Isotropic = 4,
    Metal = 5,
    Dielectric = 6,
    OrenNayar = 7,
    Emissive = 8
};

inline std::ostream& operator<<(std::ostream& out, const material_shader_model& model)
{
    if (model == material_shader_model::Lambertian)
        return out << "lambertian";
    else if (model == material_shader_model::Phong)
        return out << "phong";
    else if (model == material_shader_model::Anisotropic)
        return out << "anisotropic";
    else if (model == material_shader_model::Isotropic)
        return out << "isotropic";
    else if (model == material_shader_model::Metal)
        return out << "metal";
    else if (model == material_shader_model::Dielectric)
        return out << "dielectric";
    else if (model == material_shader_model::OrenNayar)
        return out << "oren nayar";
    else if (model == material_shader_model::Emissive)
        return out << "emissive";

    return out << "undefined";
}

static std::string to_string(std::shared_ptr<material> mat)
{
    // Check if the material is of type lambertian_material
    std::shared_ptr<lambertian_material> lm = std::dynamic_pointer_cast<lambertian_material>(mat);
    if (lm)
    {
        return "lambertian";
    }

    // Check if the material is of type phong_material
    std::shared_ptr<phong_material> pm = std::dynamic_pointer_cast<phong_material>(mat);
    if (pm)
    {
        return "phong";
    }

    // Check if the material is of type anisotropic_material
    std::shared_ptr<anisotropic_material> am = std::dynamic_pointer_cast<anisotropic_material>(mat);
    if (am)
    {
        return "anisotropic";
    }

    // Check if the material is of type isotropic_material
    std::shared_ptr<isotropic_material> im = std::dynamic_pointer_cast<isotropic_material>(mat);
    if (im)
    {
        return "isotropic";
    }

    // Check if the material is of type metal_material
    std::shared_ptr<metal_material> mm = std::dynamic_pointer_cast<metal_material>(mat);
    if (mm)
    {
        return "metal";
    }

    // Check if the material is of type dielectric_material
    std::shared_ptr<dielectric_material> dm = std::dynamic_pointer_cast<dielectric_material>(mat);
    if (dm)
    {
        return "dielectric";
    }

    // Check if the material is of type orennayar_material
    std::shared_ptr<oren_nayar_material> onm = std::dynamic_pointer_cast<oren_nayar_material>(mat);
    if (onm)
    {
        return "oren nayar";
    }

    // Check if the material is of type emissive_material
    std::shared_ptr<emissive_material> em = std::dynamic_pointer_cast<emissive_material>(mat);
    if (em)
    {
        return "emissive";
    }

    // If no type matches, return "undefined"
    return "undefined";
}
