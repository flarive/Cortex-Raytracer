#pragma once

#include "hittable.h"
#include "../misc/ray.h"
#include "../misc/hit_record.h"
#include "../utilities/types.h"
#include "../utilities/interval.h"
#include "../materials/material.h"
#include "../aabb.h"


using std::shared_ptr;
using std::string;

/// <summary>
/// Sphere primitive
/// </summary>
class sphere : public hittable
{
public:
    // Stationary Sphere
    sphere(point3 _center, double _radius, shared_ptr<material> _material, string _name = "Sphere");
    sphere(point3 _center, double _radius, shared_ptr<material> _material, const uvmapping& _mapping, string _name = "Sphere");

    // Moving Sphere
    sphere(point3 _center1, point3 _center2, double _radius, shared_ptr<material> _material, string _name = "Sphere");


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
    point3 center1{};
    double radius = 0;
    shared_ptr<material> mat;
    bool is_moving = false;
    vector3 center_vec{};

    point3 sphere_center(double time) const;


    /// <summary>
    /// Update the internal AABB of the mesh.
    /// Warning: run this when the mesh is updated.
    /// </summary>
    void updateBoundingBox() override;
};