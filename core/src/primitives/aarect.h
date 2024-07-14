#pragma once

#include "hittable.h"
#include "../misc/ray.h"
#include "../misc/hit_record.h"
#include "../materials/material.h"
#include "../utilities/interval.h"
#include "../utilities/uvmapping.h"
#include "../misc/aabb.h"

class xy_rect: public hittable 
{
    public:
        xy_rect(std::string _name = "XYRect");
        xy_rect(float _x0, float _x1, float _y0, float _y1, float _k, std::shared_ptr<material> mat, std::string _name = "XYRect");
        xy_rect(float _x0, float _x1, float _y0, float _y1, float _k, std::shared_ptr<material> mat, const uvmapping& _mapping, std::string _name = "XYRect");

        bool hit(const ray& r, interval ray_t, hit_record& rec, int depth) const override;
        aabb bounding_box() const override;


    private:

        /// <summary>
        /// Update the internal AABB of the mesh.
        /// Warning: run this when the mesh is updated.
        /// </summary>
        void updateBoundingBox() override;

        std::shared_ptr<material> mp = nullptr;
        float x0 = 0, x1 = 0, y0 = 0, y1 = 0, k = 0;
};




class xz_rect: public hittable
{
    public:
        xz_rect(std::string _name = "XZRect");
        xz_rect(float _x0, float _x1, float _z0, float _z1, float _k, std::shared_ptr<material> mat, std::string _name = "XZRect");
        xz_rect(float _x0, float _x1, float _z0, float _z1, float _k, std::shared_ptr<material> mat, const uvmapping& _mapping, std::string _name = "XZRect");

        bool hit(const ray& r, interval ray_t, hit_record& rec, int depth) const override;
        aabb bounding_box() const override;


    private:

        /// <summary>
        /// Update the internal AABB of the mesh.
        /// Warning: run this when the mesh is updated.
        /// </summary>
        void updateBoundingBox() override;

        std::shared_ptr<material> mp = nullptr;
        float x0 = 0, x1 = 0, z0 = 0, z1 = 0, k = 0;
};

class yz_rect: public hittable
{
    public:
        yz_rect(std::string _name = "YZRect");
        yz_rect(float _y0, float _y1, float _z0, float _z1, float _k, std::shared_ptr<material> mat, std::string _name = "YZRect");
        yz_rect(float _y0, float _y1, float _z0, float _z1, float _k, std::shared_ptr<material> mat, const uvmapping& _mapping, std::string _name = "YZRect");

        bool hit(const ray& r, interval ray_t, hit_record& rec, int depth) const override;
        aabb bounding_box() const override;


    private:

        /// <summary>
        /// Update the internal AABB of the mesh.
        /// Warning: run this when the mesh is updated.
        /// </summary>
        void updateBoundingBox() override;

        std::shared_ptr<material> mp = nullptr;
        float y0 = 0, y1 = 0, z0 = 0, z1 = 0, k = 0;
};