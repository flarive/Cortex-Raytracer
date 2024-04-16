#pragma once

#include "hittable.h"
#include "../misc/ray.h"
#include "../misc/hit_record.h"
#include "../utilities/interval.h"
#include "../materials/material.h"
#include "../aabb.h"

namespace raytracer
{
    /// <summary>
    /// Translate an instance
    /// </summary>
    class translate : public hittable
    {
    public:
        translate(std::shared_ptr<hittable> p, const vector3& displacement);
        bool hit(const ray& r, interval ray_t, hit_record& rec, int depth) const override;
        aabb bounding_box() const override;


    private:
        std::shared_ptr<hittable> object;
        vector3 offset;
        aabb bbox;

        /// <summary>
        /// Update the internal AABB of the mesh.
        /// Warning: run this when the mesh is updated.
        /// </summary>
        void updateBoundingBox() override;
    };
}