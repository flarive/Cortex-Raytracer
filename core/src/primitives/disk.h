#pragma once

#include "../constants.h"
#include "hittable.h"
#include "hittable_list.h"
#include "../misc/ray.h"
#include "../aabb.h"
#include "../utilities/interval.h"
#include "../misc/hit_record.h"

class disk : public hittable
{
public:
    disk();
    disk(point3 _center, double _radius, double _height, std::shared_ptr<material> _mat);

    virtual bool hit(const ray& r, interval ray_t, hit_record& rec, int depth) const override;
    virtual aabb bounding_box() const override;

public:
    point3 center;
    double radius;
    double height;
    std::shared_ptr<material> mat;
    //aabb bbox; // bounding box


    /// <summary>
    /// Update the internal AABB of the mesh.
    /// Warning: run this when the mesh is updated.
    /// </summary>
    void updateBoundingBox() override;
};