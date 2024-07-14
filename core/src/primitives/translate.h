#pragma once

#include "hittable.h"
#include "../misc/ray.h"
#include "../misc/hit_record.h"
#include "../utilities/interval.h"
#include "../materials/material.h"
#include "../misc/aabb.h"

namespace rt
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
        std::shared_ptr<hittable> m_object;
        vector3 m_offset{};

        /// <summary>
        /// Update the internal AABB of the mesh.
        /// Warning: run this when the mesh is updated.
        /// </summary>
        void updateBoundingBox() override;
    };
}