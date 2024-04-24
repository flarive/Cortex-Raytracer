#pragma once

#include "../misc/ray.h"
#include "../textures/texture.h"
#include "../materials/material.h"
#include "../misc/scatter_record.h"
#include "../primitives/hittable_list.h"
#include "../misc/hit_record.h"

/// <summary>
/// Specular material
/// </summary>
class glossy : public material
{
public:
    // Fuzz texture interpreted as the magnitude of the fuzz texture.
    glossy(std::shared_ptr<texture>& a);
    glossy(std::shared_ptr<texture>& a, std::shared_ptr<texture>& f);

    bool scatter(const ray& r_in, const hittable_list& lights, const hit_record& rec, scatter_record& srec) const override;
public:
    std::shared_ptr<texture> m_fuzz;
};