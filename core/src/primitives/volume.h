#pragma once

#include "../constants.h"
#include "hittable.h"
#include "../misc/color.h"
#include "../materials/material.h"
#include "../materials/isotropic.h"
#include "../textures/texture.h"
#include "../utilities/interval.h"
#include "../misc/hit_record.h"
#include "../pdf.h"
#include "../aabb.h"


/// <summary>
/// Constant Density Medium primitive (volume)
/// Usefull to create smoke/fog/mist...
/// </summary>
class volume : public hittable
{
public:
    volume(shared_ptr<hittable> b, double d, shared_ptr<texture> a, std::string _name = "Volume")
        : boundary(b), neg_inv_density(-1 / d), phase_function(make_shared<isotropic>(a))
    {
        name = _name;
    }

    volume(shared_ptr<hittable> b, double d, color c, std::string _name = "Volume")
        : boundary(b), neg_inv_density(-1 / d), phase_function(make_shared<isotropic>(c))
    {
        name = _name;
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec, int depth) const override;
    aabb bounding_box() const override;


private:
    shared_ptr<hittable> boundary;
    double neg_inv_density;
    shared_ptr<material> phase_function;

    /// <summary>
    /// Update the internal AABB of the mesh.
    /// Warning: run this when the mesh is updated.
    /// </summary>
    void updateBoundingBox() override;
};