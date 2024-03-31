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
        xy_rect(std::string _name = "XYRect");
        xy_rect(float _x0, float _x1, float _y0, float _y1, float _k, std::shared_ptr<material> mat, std::string _name = "XYRect");

        bool hit(const ray& r, interval ray_t, hit_record& rec, int depth) const override;
        aabb bounding_box() const override;


    private:

        /// <summary>
        /// Update the internal AABB of the mesh.
        /// Warning: run this when the mesh is updated.
        /// </summary>
        void updateBoundingBox() override;

        std::shared_ptr<material> mp;
        float x0, x1, y0, y1, k = 0;
        aabb bbox;
};




class xz_rect: public hittable
{
    public:
        xz_rect(std::string _name = "XZRect");
        xz_rect(float _x0, float _x1, float _z0, float _z1, float _k, std::shared_ptr<material> mat, std::string _name = "XZRect");

        bool hit(const ray& r, interval ray_t, hit_record& rec, int depth) const override;
        aabb bounding_box() const override;


    private:

        /// <summary>
        /// Update the internal AABB of the mesh.
        /// Warning: run this when the mesh is updated.
        /// </summary>
        void updateBoundingBox() override;

        std::shared_ptr<material> mp;
        float x0, x1, z0, z1, k = 0;
        aabb bbox;
};

class yz_rect: public hittable
{
    public:
        yz_rect(std::string _name = "YZRect");
        yz_rect(float _y0, float _y1, float _z0, float _z1, float _k, std::shared_ptr<material> mat, std::string _name = "YZRect");

        bool hit(const ray& r, interval ray_t, hit_record& rec, int depth) const override;
        aabb bounding_box() const override;


    private:

        /// <summary>
        /// Update the internal AABB of the mesh.
        /// Warning: run this when the mesh is updated.
        /// </summary>
        void updateBoundingBox() override;

        std::shared_ptr<material> mp;
        float y0, y1, z0, z1, k = 0;
        aabb bbox;
};