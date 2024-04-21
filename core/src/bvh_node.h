#pragma once

#include "misc/ray.h"
#include "misc/hit_record.h"
#include "primitives/hittable.h"
#include "primitives/hittable_list.h"
#include "utilities/math_utils.h"
#include "utilities/interval.h"
#include "aabb.h"

#include <algorithm>

/// <summary>
/// Bounding volume hierarchy
/// </summary>
class bvh_node : public hittable
{
public:
    bvh_node(const hittable_list& list);
    bvh_node(const std::vector<std::shared_ptr<hittable>>& src_objects, size_t start, size_t end);

    bool hit(const ray& r, interval ray_t, hit_record& rec, int depth) const override;
    aabb bounding_box() const override;

private:
    std::shared_ptr<hittable> left;
    std::shared_ptr<hittable> right;
    aabb bbox;

    /// <summary>
    /// Update the internal AABB of the mesh.
    /// Warning: run this when the mesh is updated.
    /// </summary>
    void updateBoundingBox() override;


    static bool box_compare(const std::shared_ptr<hittable> a, const std::shared_ptr<hittable> b, int axis_index);
    static bool box_x_compare(const std::shared_ptr<hittable> a, const std::shared_ptr<hittable> b);
    static bool box_y_compare(const std::shared_ptr<hittable> a, const std::shared_ptr<hittable> b);
    static bool box_z_compare(const std::shared_ptr<hittable> a, const std::shared_ptr<hittable> b);
};