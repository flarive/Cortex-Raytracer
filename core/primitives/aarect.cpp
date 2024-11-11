#include "aarect.h"

#include "../utilities/uvmapping.h"

xy_rect::xy_rect(std::string _name)
{
    m_name = _name;
}

xy_rect::xy_rect(float _x0, float _x1, float _y0, float _y1, float _k, std::shared_ptr<material> mat, std::string _name)
    : xy_rect(_x0, _x1, _y0, _y1, k, mat, uvmapping(), _name)
{
}

xy_rect::xy_rect(float _x0, float _x1, float _y0, float _y1, float _k, std::shared_ptr<material> mat, const uvmapping& _mapping, std::string _name)
    : x0(_x0), x1(_x1), y0(_y0), y1(_y1), k(_k), mp(mat)
{
    m_name = _name;
    m_mapping = _mapping;

    m_bbox = aabb(point3(x0, y0, k - 0.0001), point3(x1, y1, k + 0.0001));
}

bool xy_rect::hit(const ray& r, interval ray_t, hit_record& rec, int depth, randomizer& rnd) const
{
    double t = (k - r.origin().z) / r.direction().z;
    if (t < ray_t.min || t > ray_t.max)
        return false;
    double x = r.origin().x + t * r.direction().x;
    double y = r.origin().y + t * r.direction().y;
    if (x < x0 || x > x1 || y < y0 || y > y1)
        return false;

    // UV coordinates
    get_xy_rect_uv(x, y, rec.u, rec.v, x0, x1, y0, y1, m_mapping);

    rec.t = t;
    rec.mat = mp;
    rec.name = m_name;
    rec.bbox = m_bbox;
    rec.hit_point = r.at(t);
    rec.normal = vector3(0, 0, 1);
    return true;
}

aabb xy_rect::bounding_box() const
{
    return m_bbox;
}


/// <summary>
/// Update the internal AABB of the mesh.
/// Warning: run this when the mesh is updated.
/// </summary>
void xy_rect::updateBoundingBox()
{
    // to implement
}



xz_rect::xz_rect(std::string _name)
{
    m_name = _name;
}

xz_rect::xz_rect(float _x0, float _x1, float _z0, float _z1, float _k, std::shared_ptr<material> mat, std::string _name)
    : xz_rect(_x0, _x1, _z0, _z1, _k, mat, uvmapping(), _name)
{
}

xz_rect::xz_rect(float _x0, float _x1, float _z0, float _z1, float _k, std::shared_ptr<material> mat, const uvmapping& _mapping, std::string _name)
    : x0(_x0), x1(_x1), z0(_z0), z1(_z1), k(_k), mp(mat)
{
    m_name = _name;
    m_mapping = _mapping;

    m_bbox = aabb(vector3(x0, k - 0.0001, z0), vector3(x1, k + 0.0001, z1));
}

bool xz_rect::hit(const ray& r, interval ray_t, hit_record& rec, int depth, randomizer& rnd) const
{
    double t = (k - r.origin().y) / r.direction().y;
    if (t < ray_t.min || t > ray_t.max)
        return false;
    double x = r.origin().x + t * r.direction().x;
    double z = r.origin().z + t * r.direction().z;
    if (x < x0 || x > x1 || z < z0 || z > z1)
        return false;

	// UV coordinates
    get_xz_rect_uv(x, z, rec.u, rec.v, x0, x1, z0, z1, m_mapping);

    rec.t = t;
    rec.mat = mp;
    rec.name = m_name;
    rec.bbox = m_bbox;
    rec.hit_point = r.at(t);
    rec.normal = vector3(0, 1, 0);
    return true;
}

aabb xz_rect::bounding_box() const
{
    return m_bbox;
}


/// <summary>
/// Update the internal AABB of the mesh.
/// Warning: run this when the mesh is updated.
/// </summary>
void xz_rect::updateBoundingBox()
{
    // to implement
}


yz_rect::yz_rect(std::string _name)
{
    m_name = _name;
}

yz_rect::yz_rect(float _y0, float _y1, float _z0, float _z1, float _k, std::shared_ptr<material> mat, std::string _name)
    : yz_rect(_y0, _y1, _z0, _z1, _k, mat, uvmapping(), _name)
{
}

yz_rect::yz_rect(float _y0, float _y1, float _z0, float _z1, float _k, std::shared_ptr<material> mat, const uvmapping& _mapping, std::string _name)
    : y0(_y0), y1(_y1), z0(_z0), z1(_z1), k(_k), mp(mat)
{
    m_name = _name;
    m_mapping = _mapping;

    m_bbox = aabb(vector3(k - 0.0001, y0, z0), vector3(k + 0.0001, y1, z1));
}


bool yz_rect::hit(const ray& r, interval ray_t, hit_record& rec, int depth, randomizer& rnd) const
{
    double t = (k - r.origin().x) / r.direction().x;
    if (t < ray_t.min || t > ray_t.max)
        return false;
    double y = r.origin().y + t * r.direction().y;
    double z = r.origin().z + t * r.direction().z;
    if (y < y0 || y > y1 || z < z0 || z > z1)
        return false;

	// UV coordinates
    get_yz_rect_uv(y, z, rec.u, rec.v, y0, y1, z0, z1, m_mapping);

    rec.t = t;
    rec.mat = mp;
    rec.name = m_name;
    rec.bbox = m_bbox;
    rec.hit_point = r.at(t);
    rec.normal = vector3(1, 0, 0);
    return true;
}

aabb yz_rect::bounding_box() const
{
    return m_bbox;
}


/// <summary>
/// Update the internal AABB of the mesh.
/// Warning: run this when the mesh is updated.
/// </summary>
void yz_rect::updateBoundingBox()
{
    // to implement
}
