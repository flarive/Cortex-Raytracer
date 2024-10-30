#pragma once

#include "../constants.h"
#include "hittable.h"
#include "../misc/color.h"
#include "../materials/material.h"
#include "../materials/isotropic.h"
#include "../textures/texture.h"
#include "../utilities/interval.h"
#include "../misc/hit_record.h"
#include "../misc/aabb.h"


/// <summary>
/// Constant Density Medium primitive (volume)
/// Usefull to create smoke/fog/mist...
/// </summary>
class volume : public hittable
{
public:
    volume(std::shared_ptr<hittable> boundary, double density, std::shared_ptr<texture> tex, std::string _name = "Volume");
    volume(std::shared_ptr<hittable> boundary, double density, color c, std::string _name = "Volume");

    bool hit(const ray& r, interval ray_t, hit_record& rec, int depth, randomizer& rnd) const override;
    aabb bounding_box() const override;


private:
    std::shared_ptr<hittable> m_boundary = nullptr;
    double m_neg_inv_density = 0.0;
    std::shared_ptr<material> m_phase_function = nullptr;

    /// <summary>
    /// Update the internal AABB of the mesh.
    /// Warning: run this when the mesh is updated.
    /// </summary>
    void updateBoundingBox() override;
};