#include "box.h"

#include "../utilities/uvmapping.h"
#include "flip_normals.h"
#include "../aabb.h"
#include "aarect.h"

using namespace rt;

box::box(const vector3& _center, const vector3& _size, std::shared_ptr<material> _mat, std::string _name)
    : box(_center, _size, _mat, uvmapping(), _name)
{
}

box::box(const vector3& _center, const vector3& _size, std::shared_ptr<material> _mat, const uvmapping& _mapping, std::string _name)
{
    pmin = vector3(_center.x - (_size.x / 2.0), _center.y - (_size.y / 2.0), _center.z - (_size.z / 2.0));
    pmax = pmin + _size;

    list_ptr = std::make_shared< hittable_list>();
    
    // font face
    list_ptr->add(std::make_shared<xy_rect>(pmin.x, pmax.x, pmin.y, pmax.y, pmax.z, _mat, _mapping));

    // back face
    list_ptr->add(std::make_shared<flip_normals>(std::make_shared<xy_rect>(pmin.x, pmax.x, pmin.y, pmax.y, pmin.z, _mat, _mapping)));

    // top face
    list_ptr->add(std::make_shared<xz_rect>(pmin.x, pmax.x, pmin.z, pmax.z, pmax.y, _mat, _mapping));
    
    // bottom face
    list_ptr->add(std::make_shared<flip_normals>(std::make_shared<xz_rect>(pmin.x, pmax.x, pmin.z, pmax.z, pmin.y, _mat, _mapping)));
    
    // right face
    list_ptr->add(std::make_shared<yz_rect>(pmin.y, pmax.y, pmin.z, pmax.z, pmax.x, _mat, _mapping));
    
    // left facd
    list_ptr->add(std::make_shared<flip_normals>(std::make_shared<yz_rect>(pmin.y, pmax.y, pmin.z, pmax.z, pmin.x, _mat, _mapping)));

    m_bbox = aabb(pmin, pmax);
}

bool box::hit(const ray& r, interval ray_t, hit_record& rec, int depth) const
{
    return list_ptr->hit(r, ray_t, rec, depth);
}

aabb box::bounding_box() const
{
    return m_bbox;
}

/// <summary>
/// Update the internal AABB of the mesh.
/// Warning: run this when the mesh is updated.
/// </summary>
void box::updateBoundingBox()
{
    // to implement
}