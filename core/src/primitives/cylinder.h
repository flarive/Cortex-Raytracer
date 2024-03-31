#pragma once

#include "../constants.h"
#include "hittable.h"
#include "hittable_list.h"
#include "../misc/ray.h"
#include "../aabb.h"
#include "../utilities/interval.h"
#include "../misc/hit_record.h"

/// <summary>
/// Cylinder primitive
/// </summary>
class cylinder : public hittable
{
public:
    cylinder(std::string _name = "Cylinder");
    cylinder(point3 _center, double _radius, double _height, std::string _name = "Cylinder");
    cylinder(point3 _center, double _radius, double _height, std::shared_ptr<material> _material, std::string _name = "Cylinder");

    bool hit(const ray& r, interval ray_t, hit_record& rec, int depth) const;
 
    aabb bounding_box() const override;


private:
    point3 center;
    double radius;
    double height;
    std::shared_ptr<material> mat;
    aabb bbox; // bounding box

    void get_cylinder_uv(const vector3& p, double& u, double& v, double radius) const;

    /// <summary>
    /// Update the internal AABB of the mesh.
    /// Warning: run this when the mesh is updated.
    /// </summary>
    void updateBoundingBox() override;
};