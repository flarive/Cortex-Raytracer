#pragma once

#include "../misc/ray.h"
#include "hittable_list.h"
#include "../misc/hit_record.h"
#include "../materials/material.h"
#include "../utilities/interval.h"
#include "../utilities/uvmapping.h"

class box: public hittable
{
    public:
        box(std::string _name = "Box");
        box(const vector3& _center, const vector3& _size, std::shared_ptr<material> _mat, std::string _name = "Box");
        box(const vector3& _center, const vector3& _size, std::shared_ptr<material> _mat, const uvmapping& _mapping, std::string _name = "Box");

        bool hit(const ray& r, interval ray_t, hit_record& rec, int depth, randomizer& rnd) const override;
        aabb bounding_box() const override;


        /// <summary>
        /// Update the internal AABB of the mesh.
        /// Warning: run this when the mesh is updated.
        /// </summary>
        void updateBoundingBox() override;

    private:
        vector3 pmin{}, pmax{};
        std::shared_ptr<hittable_list> list_ptr;
};