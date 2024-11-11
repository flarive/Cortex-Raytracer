#pragma once

#include "../misc/aabb.h"
#include "../misc/hit_record.h"
#include "../misc/ray.h"
#include "../utilities/types.h"
#include "../utilities/interval.h"
#include "../randomizers/randomize.h"
#include "hittable.h"

#include <memory>
#include <vector>

/// <summary>
/// List of hittable primitives
/// </summary>
class hittable_list : public hittable
{
public:
    std::vector<std::shared_ptr<hittable>> objects;

    hittable_list(std::string _name = "HittableList");
    hittable_list(std::shared_ptr<hittable> object, std::string _name = "HittableList");

    void clear();
    std::shared_ptr<hittable>& back();

    void add(std::shared_ptr<hittable> object);
    std::shared_ptr<hittable>& get(const std::string& name);
    bool remove(std::shared_ptr<hittable> object);

    bool hit(const ray& r, interval ray_t, hit_record& rec, int depth, randomizer& rnd) const override;


    aabb bounding_box() const override;


    double pdf_value(const point3& o, const vector3& v, randomizer& rnd) const override;


    /// <summary>
    /// Random special implementation for hittable list (override base)
    /// </summary>
    /// <param name="origin"></param>
    /// <returns></returns>
    vector3 random(const vector3& o, randomizer& rnd) const override;



private:
    /// <summary>
    /// Update the internal AABB of the mesh.
    /// Warning: run this when the mesh is updated.
    /// </summary>
    void updateBoundingBox() override;
};