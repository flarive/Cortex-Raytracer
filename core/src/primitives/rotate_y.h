#pragma once

#include "hittable.h"
#include "../misc/ray.h"
#include "../utilities/interval.h"
#include "../materials/material.h"
#include "../aabb.h"
#include "../misc/hit_record.h"


/// <summary>
/// Rotate an instance
/// </summary>
class rotate_y : public hittable
{
public:
    rotate_y(std::shared_ptr<hittable> p, double angle);
    bool hit(const ray& r, interval ray_t, hit_record& rec, int depth) const override;
    aabb bounding_box() const override;


private:
    std::shared_ptr<hittable> object;
    double sin_theta;
    double cos_theta;
    aabb bbox;

    /// <summary>
    /// Update the internal AABB of the mesh.
    /// Warning: run this when the mesh is updated.
    /// </summary>
    void updateBoundingBox() override;
};