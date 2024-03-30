#pragma once

#include "hittable.h"
#include "../misc/ray.h"
#include "../misc/hit_record.h"
#include "../materials/material.h"
#include "../utilities/types.h"
#include "../utilities/interval.h"
#include "../aabb.h"

class xy_rect: public hittable 
{
    public:
        xy_rect(std::string _name = "XYRect")
        {
            name = _name;
        }

        xy_rect(float _x0, float _x1, float _y0, float _y1, float _k, std::shared_ptr<material> mat, std::string _name = "XYRect")
            : x0(_x0), x1(_x1), y0(_y0), y1(_y1), k(_k), mp(mat)
        {
            name = _name;
            
            bbox = aabb(point3(x0, y0, k - 0.0001), point3(x1, y1, k + 0.0001));
        }

        bool hit(const ray& r, interval ray_t, hit_record& rec, int depth) const override;
        aabb bounding_box() const override;


    private:

        /// <summary>
        /// Update the internal AABB of the mesh.
        /// Warning: run this when the mesh is updated.
        /// </summary>
        void updateBoundingBox() override;

        std::shared_ptr<material>  mp;
        float x0, x1, y0, y1, k;
        aabb bbox;
};




class xz_rect: public hittable
{
    public:
        xz_rect(std::string _name = "XZRect")
        {
            name = _name;
        }

        xz_rect(float _x0, float _x1, float _z0, float _z1, float _k, std::shared_ptr<material> mat, std::string _name = "XZRect")
            : x0(_x0), x1(_x1), z0(_z0), z1(_z1), k(_k), mp(mat)
        {
            name = _name;
            
            bbox = aabb(vector3(x0, k - 0.0001, z0), vector3(x1, k + 0.0001, z1));
        }

        bool hit(const ray& r, interval ray_t, hit_record& rec, int depth) const override;
        aabb bounding_box() const override;


    private:

        /// <summary>
        /// Update the internal AABB of the mesh.
        /// Warning: run this when the mesh is updated.
        /// </summary>
        void updateBoundingBox() override;

        std::shared_ptr<material>  mp;
        float x0, x1, z0, z1, k;
        aabb bbox;
};

class yz_rect: public hittable
{
    public:
        yz_rect(std::string _name = "YZRect")
        {
            name = _name;
        }

        yz_rect(float _y0, float _y1, float _z0, float _z1, float _k, std::shared_ptr<material> mat, std::string _name = "YZRect")
            : y0(_y0), y1(_y1), z0(_z0), z1(_z1), k(_k), mp(mat)
        {
            name = _name;
            
            bbox = aabb(vector3(k - 0.0001, y0, z0), vector3(k + 0.0001, y1, z1));
        }


        bool hit(const ray& r, interval ray_t, hit_record& rec, int depth) const override;
        aabb bounding_box() const override;


    private:

        /// <summary>
        /// Update the internal AABB of the mesh.
        /// Warning: run this when the mesh is updated.
        /// </summary>
        void updateBoundingBox() override;

        std::shared_ptr<material>  mp;
        float y0, y1, z0, z1, k;
        aabb bbox;
};