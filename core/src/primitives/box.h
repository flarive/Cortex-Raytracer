#pragma once

#include "aarect.h"
#include "../misc/ray.h"
#include "hittable_list.h"
#include "../misc/hit_record.h"
#include "../materials/material.h"
#include "flip_normals.h"
#include "../aabb.h"
#include "../utilities/interval.h"

class box: public hittable
{
    public:
        box(std::string _name = "Box") {}
        box(const vector3& p0, const vector3& p1, std::shared_ptr<material> mat, std::string _name = "Box");

        bool hit(const ray& r, interval ray_t, hit_record& rec, int depth) const override;
        aabb bounding_box() const override;


        /// <summary>
        /// Update the internal AABB of the mesh.
        /// Warning: run this when the mesh is updated.
        /// </summary>
        void updateBoundingBox() override;

    private:
        vector3 pmin{}, pmax{};
        std::shared_ptr<hittable_list> list_ptr;
        //aabb bbox;
};