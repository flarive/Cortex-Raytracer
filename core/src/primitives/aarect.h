#ifndef AARECT_H
#define AARECT_H

#include "hittable.h"

class xy_rect: public hittable 
{
    public:
        xy_rect(string _name = "XYRect")
        {
            name = _name;
        }

        xy_rect(float _x0, float _x1, float _y0, float _y1, float _k, shared_ptr<material> mat, string _name = "XYRect")
            : x0(_x0), x1(_x1), y0(_y0), y1(_y1), k(_k), mp(mat)
        {
            name = _name;
            
            bbox = aabb(point3(x0, y0, k - 0.0001), point3(x1, y1, k + 0.0001));
        }

        bool hit(const ray& r, interval ray_t, hit_record& rec, int depth) const override
        {
            float t = (k - r.origin().z) / r.direction().z;
            if (t < ray_t.min || t > ray_t.max)
                return false;
            float x = r.origin().x + t * r.direction().x;
            float y = r.origin().y + t * r.direction().y;
            if (x < x0 || x > x1 || y < y0 || y > y1)
                return false;
            rec.u = (x - x0) / (x1 - x0);
            rec.v = (y - y0) / (y1 - y0);
            rec.t = t;
            rec.mat = mp;
            rec.name = name;
            rec.bbox = bbox;
            rec.p = r.at(t);
            rec.normal = vector3(0, 0, 1);
            return true;
        }

        aabb bounding_box() const override
        {
            return bbox;
        }

    private:

        /// <summary>
        /// Update the internal AABB of the mesh.
        /// Warning: run this when the mesh is updated.
        /// </summary>
        void updateBoundingBox() override
        {
            // to implement
        }

        shared_ptr<material>  mp;
        float x0, x1, y0, y1, k;
        aabb bbox;
};




class xz_rect: public hittable
{
    public:
        xz_rect(string _name = "XZRect")
        {
            name = _name;
        }

        xz_rect(float _x0, float _x1, float _z0, float _z1, float _k, shared_ptr<material> mat, string _name = "XZRect")
            : x0(_x0), x1(_x1), z0(_z0), z1(_z1), k(_k), mp(mat)
        {
            name = _name;
            
            bbox = aabb(vector3(x0, k - 0.0001, z0), vector3(x1, k + 0.0001, z1));
        }

        bool hit(const ray& r, interval ray_t, hit_record& rec, int depth) const override
        {
            float t = (k - r.origin().y) / r.direction().y;
            if (t < ray_t.min || t > ray_t.max)
                return false;
            float x = r.origin().x + t * r.direction().x;
            float z = r.origin().z + t * r.direction().z;
            if (x < x0 || x > x1 || z < z0 || z > z1)
                return false;
            rec.u = (x - x0) / (x1 - x0);
            rec.v = (z - z0) / (z1 - z0);
            rec.t = t;
            rec.mat = mp;
            rec.name = name;
            rec.bbox = bbox;
            rec.p = r.at(t);
            rec.normal = vector3(0, 1, 0);
            return true;
        }

        aabb bounding_box() const override
        {
            return bbox;
        }

    private:

        /// <summary>
        /// Update the internal AABB of the mesh.
        /// Warning: run this when the mesh is updated.
        /// </summary>
        void updateBoundingBox() override
        {
            // to implement
        }

        shared_ptr<material>  mp;
        float x0, x1, z0, z1, k;
        aabb bbox;
};

class yz_rect: public hittable
{
    public:
        yz_rect(string _name = "YZRect")
        {
            name = _name;
        }

        yz_rect(float _y0, float _y1, float _z0, float _z1, float _k, shared_ptr<material> mat, string _name = "YZRect")
            : y0(_y0), y1(_y1), z0(_z0), z1(_z1), k(_k), mp(mat)
        {
            name = _name;
            
            bbox = aabb(vector3(k - 0.0001, y0, z0), vector3(k + 0.0001, y1, z1));
        }


        bool hit(const ray& r, interval ray_t, hit_record& rec, int depth) const override
        {
            float t = (k - r.origin().x) / r.direction().x;
            if (t < ray_t.min || t > ray_t.max)
                return false;
            float y = r.origin().y + t * r.direction().y;
            float z = r.origin().z + t * r.direction().z;
            if (y < y0 || y > y1 || z < z0 || z > z1)
                return false;
            rec.u = (y - y0) / (y1 - y0);
            rec.v = (z - z0) / (z1 - z0);
            rec.t = t;
            rec.mat = mp;
            rec.name = name;
            rec.bbox = bbox;
            rec.p = r.at(t);
            rec.normal = vector3(1, 0, 0);
            return true;
        }

        aabb bounding_box() const override
        {
            return bbox;
        }

    private:

        /// <summary>
        /// Update the internal AABB of the mesh.
        /// Warning: run this when the mesh is updated.
        /// </summary>
        void updateBoundingBox() override
        {
            // to implement
        }

        shared_ptr<material>  mp;
        float y0, y1, z0, z1, k;
        aabb bbox;
};

#endif
