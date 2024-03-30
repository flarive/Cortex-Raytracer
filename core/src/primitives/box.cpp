#include "box.h"

box::box(const vector3& p0, const vector3& p1, shared_ptr<material> mat, std::string _name)
{
    pmin = p0;
    pmax = p1;

    list_ptr = make_shared< hittable_list>();

    list_ptr->add(std::make_shared<xy_rect>(p0.x, p1.x, p0.y, p1.y, p1.z, mat));
    list_ptr->add(std::make_shared<flip_normals>(std::make_shared<xy_rect>(p0.x, p1.x, p0.y, p1.y, p0.z, mat)));
    list_ptr->add(std::make_shared<xz_rect>(p0.x, p1.x, p0.z, p1.z, p1.y, mat));
    list_ptr->add(std::make_shared<flip_normals>(std::make_shared<xz_rect>(p0.x, p1.x, p0.z, p1.z, p0.y, mat)));
    list_ptr->add(std::make_shared<yz_rect>(p0.y, p1.y, p0.z, p1.z, p1.x, mat));
    list_ptr->add(std::make_shared<flip_normals>(std::make_shared<yz_rect>(p0.y, p1.y, p0.z, p1.z, p0.x, mat)));

    bbox = aabb(pmin, pmax);
}

bool box::hit(const ray& r, interval ray_t, hit_record& rec, int depth) const
{
    return list_ptr->hit(r, ray_t, rec, depth);
}

aabb box::bounding_box() const
{
    return bbox;
}

/// <summary>
/// Update the internal AABB of the mesh.
/// Warning: run this when the mesh is updated.
/// </summary>
void box::updateBoundingBox()
{
    // to implement
}