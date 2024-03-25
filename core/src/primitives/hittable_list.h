#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include "hittable.h"
#include "aabb.h"

#include <memory>
#include <vector>

using std::shared_ptr;
using std::make_shared;

/// <summary>
/// List of hittable primitives
/// </summary>
class hittable_list : public hittable
{
public:
    std::vector<shared_ptr<hittable>> objects;

    hittable_list(string _name = "HittableList")
    {
        name = _name;
    }

    hittable_list(shared_ptr<hittable> object, string _name = "HittableList")
    {
        name = _name;

        add(object);
    }

    void clear()
    {
        objects.clear();
    }

    void add(shared_ptr<hittable> object)
    {
        objects.push_back(object);
        bbox = aabb(bbox, object->bounding_box());
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec, int depth) const override
    {
        hit_record temp_rec;
        bool hit_anything = false;
        auto closest_so_far = ray_t.max;

        for (const auto& object : objects)
        {
            if (object->hit(r, interval(ray_t.min, closest_so_far), temp_rec, depth))
            {
                hit_anything = true;
                closest_so_far = temp_rec.t;
                rec = temp_rec;
            }
        }

        return hit_anything;
    }

    aabb bounding_box() const override
    {
        return bbox;
    }

    double pdf_value(const point3& o, const vector3& v) const override
    {
        auto weight = 1.0 / objects.size();
        auto sum = 0.0;

        for (const auto& object : objects)
        {
            sum += weight * object->pdf_value(o, v);
        }

        return sum;
    }

    /// <summary>
    /// Random special implementation for hittable list (override base)
    /// </summary>
    /// <param name="origin"></param>
    /// <returns></returns>
    vector3 random(const vector3& o) const override
    {
        auto int_size = static_cast<int>(objects.size());
        return objects[random_int(0, int_size - 1)]->random(o);
    }


private:
    aabb bbox;

    //// <summary>
    /// Update the internal AABB of the mesh.
    /// Warning: run this when the mesh is updated.
    /// </summary>
    void updateBoundingBox() override
    {
        // to implement
    }
};

#endif