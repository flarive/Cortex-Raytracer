#pragma once

#include "hittable.h"
#include "../misc/ray.h"
#include "../misc/hit_record.h"
#include "../utilities/types.h"
#include "../utilities/interval.h"
#include "../utilities/math_utils.h"
#include "../materials/material.h"
#include "../aabb.h"
#include "../onb.h"

using std::shared_ptr;
using std::string;

/// <summary>
/// Sphere primitive
/// </summary>
class sphere : public hittable
{
public:
    // Stationary Sphere
    sphere(point3 _center, double _radius, shared_ptr<material> _material, string _name = "Sphere")
        : center1(_center), radius(_radius), mat(_material), is_moving(false)
    {
        name = _name;
        
        // calculate stationary sphere bounding box for ray optimizations
        vector3 rvec = vector3(radius, radius, radius);
        bbox = aabb(center1 - rvec, center1 + rvec);
    }



    // Moving Sphere
    sphere(point3 _center1, point3 _center2, double _radius, shared_ptr<material> _material, string _name = "Sphere")
        : center1(_center1), radius(_radius), mat(_material), is_moving(true)
    {
        name = _name;
        
        // calculate moving sphere bounding box for ray optimizations
        vector3 rvec = vector3(radius, radius, radius);
        aabb box1(_center1 - rvec, _center1 + rvec);
        aabb box2(_center2 - rvec, _center2 + rvec);
        bbox = aabb(box1, box2);

        center_vec = _center2 - _center1;
    }



    aabb bounding_box() const override;


    /// <summary>
    /// Logic of sphere ray hit detection
    /// </summary>
    /// <param name="r"></param>
    /// <param name="ray_t"></param>
    /// <param name="rec"></param>
    /// <returns></returns>
    bool hit(const ray& r, interval ray_t, hit_record& rec, int depth) const override;
        

    double pdf_value(const point3& o, const vector3& v) const override;
   

    /// <summary>
    /// Random special implementation for sphere (override base)
    /// </summary>
    /// <param name="origin"></param>
    /// <returns></returns>
    vector3 random(const point3& o) const override;


private:
    point3 center1;
    double radius;
    shared_ptr<material> mat;
    bool is_moving;
    vector3 center_vec;
    aabb bbox; // bounding box

    point3 sphere_center(double time) const;


    /// <summary>
    /// Update the internal AABB of the mesh.
    /// Warning: run this when the mesh is updated.
    /// </summary>
    void updateBoundingBox() override;
};