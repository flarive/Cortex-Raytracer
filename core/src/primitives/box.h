#pragma once

#include "aarect.h"
#include "hittable_list.h"

class box: public hittable
{
    public:
        box(string _name = "Box") {}
        box(const vector3& p0, const vector3& p1, shared_ptr<material> mat, string _name = "Box")
        {
            pmin = p0;
            pmax = p1;

            list_ptr = make_shared< hittable_list>();

            list_ptr->add(std::make_shared<xy_rect>(p0.x, p1.x, p0.y, p1.y, p1.z, mat));
            list_ptr->add(std::make_shared<flip_normals>(std::make_shared<xy_rect>(p0.x, p1.x, p0.y, p1.y, p0.z, mat)));
            list_ptr->add(std::make_shared<xz_rect>(p0.x, p1.x, p0.z, p1.z, p1.y, mat));
            list_ptr->add(std::make_shared<flip_normals>(std::make_shared<xz_rect>(p0.x, p1.x, p0.z, p1.z, p0.y, mat)));
            list_ptr->add(std::make_shared<yz_rect>(p0.y, p1.y, p0.z, p1.z, p1.x, mat));
            list_ptr->add(std::make_shared<flip_normals>(std::make_shared<yz_rect>(p0.y, p1.y, p0.z, p1.z, p0.x, mat)));
  
            bbox = aabb(pmin, pmax);
        }

        bool hit(const ray& r, interval ray_t, hit_record& rec, int depth) const override
        {
            return list_ptr->hit(r, ray_t, rec, depth);
        }

        aabb bounding_box() const override
        {
            return bbox;
        }

        /// <summary>
        /// Update the internal AABB of the mesh.
        /// Warning: run this when the mesh is updated.
        /// </summary>
        void updateBoundingBox() override
        {
            // to implement
        }

    private:
        vector3 pmin, pmax;
        shared_ptr<hittable_list> list_ptr;
        aabb bbox;
};