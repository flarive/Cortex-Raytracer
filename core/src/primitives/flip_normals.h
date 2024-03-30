#pragma once

#include "hittable.h"
#include "../misc/ray.h"
#include "../misc/hit_record.h"
#include "../utilities/interval.h"
#include "../materials/material.h"
#include "../aabb.h"

class flip_normals : public hittable
{
public:
    flip_normals(shared_ptr<hittable> p) : object(p)
    {
        bbox = object->bounding_box();
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec, int depth) const override
    {
        if (object->hit(r, ray_t, rec, depth))
        {
            rec.normal = -rec.normal;
            return true;
        }
        else
            return false;
    }


    aabb bounding_box() const override
    {
        return bbox;
    }

private:
    shared_ptr<hittable> object;
    aabb bbox;

    /// <summary>
    /// Update the internal AABB of the mesh.
    /// Warning: run this when the mesh is updated.
    /// </summary>
    void updateBoundingBox() override
    {
        // to implement
    }
};