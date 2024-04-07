#pragma once

#include "../constants.h"
#include "../misc/ray.h"
#include "../misc/color.h"
#include "../textures/texture.h"
#include "../primitives/hittable.h"
#include "../materials/material.h"
#include "../primitives/hittable_list.h"
#include "../misc/scatter_record.h"

/// <summary>
/// Glossy material (NOT TESTED !!!)
/// https://github.com/Drummersbrother/raytracing-in-one-weekend/blob/master/material.h
/// </summary>
class glossy : public material
{
public:
    // Fuzz texture interpreted as the magnitude of the fuzz texture.
    glossy(std::shared_ptr<texture>& _albedo, std::shared_ptr<texture>& _fuzz);

    virtual bool scatter(const ray& ray_in, const hittable_list& lights, const hit_record& rec, scatter_record& srec) const override;

public:
    std::shared_ptr<texture> m_fuzz;
};