#include "sphere.h"

#include "../constants.h"
#include "../onb.h"
#include "../utilities/uvmapping.h"
#include "../utilities/randomizer.h"
#include "../misc/singleton.h"

#include <glm/glm.hpp>

using std::shared_ptr;
using std::string;


sphere::sphere(point3 _center, double _radius, shared_ptr<material> _material, string _name)
    : sphere(_center, _radius, _material, uvmapping(), _name)
{
    is_moving = false;
}

sphere::sphere(point3 _center1, point3 _center2, double _radius, shared_ptr<material> _material, string _name)
    : center1(_center1), radius(_radius), mat(_material), is_moving(true)
{
    m_name = _name;

    // calculate moving sphere bounding box for ray optimizations
    vector3 rvec = vector3(radius, radius, radius);
    aabb box1(_center1 - rvec, _center1 + rvec);
    aabb box2(_center2 - rvec, _center2 + rvec);
    m_bbox = aabb(box1, box2);

    center_vec = _center2 - _center1;
}

sphere::sphere(point3 _center, double _radius, shared_ptr<material> _material, const uvmapping& _mapping, string _name)
    : center1(_center), radius(_radius), mat(_material), is_moving(false)
{
    m_name = _name;
    m_mapping = _mapping;

    // calculate stationary sphere bounding box for ray optimizations
    vector3 rvec = vector3(radius, radius, radius);
    m_bbox = aabb(center1 - rvec, center1 + rvec);
}

aabb sphere::bounding_box() const
{
    return m_bbox;
}

/// <summary>
/// Logic of sphere ray hit detection
/// </summary>
/// <param name="r"></param>
/// <param name="ray_t"></param>
/// <param name="rec"></param>
/// <returns></returns>
bool sphere::hit(const ray& r, interval ray_t, hit_record& rec, int depth) const
{
    point3 center = is_moving ? sphere_center(r.time()) : center1;
    vector3 oc = r.origin() - center;
    auto a = vector_length_squared(r.direction());
    auto half_b = glm::dot(oc, r.direction());
    auto c = vector_length_squared(oc) - radius * radius;

    auto discriminant = half_b * half_b - a * c;
    if (discriminant < 0) return false;
    auto sqrtd = sqrt(discriminant);

    // Find the nearest root that lies in the acceptable range.
    auto root = (-half_b - sqrtd) / a;
    if (!ray_t.surrounds(root)) {
        root = (-half_b + sqrtd) / a;
        if (!ray_t.surrounds(root))
            return false;
    }

    // number of hits encountered by the ray (only the nearest ?)
    rec.t = root;

    // point coordinate of the hit
    rec.hit_point = r.at(rec.t);

    // material of the hit object
    rec.mat = mat;

    // name of the primitive hit by the ray
    rec.name = m_name;
    rec.bbox = m_bbox;

    // set normal and front-face tracking
    vector3 outward_normal = (rec.hit_point - center) / radius;
    rec.set_face_normal(r, outward_normal);

    // compute phi and theta for tangent and bitangent calculation
    double phi = atan2(outward_normal.z, outward_normal.x);
    double theta = acos(outward_normal.y);

    // compute sphere primitive tangent and bitangent for normals
    vector3 tan, bitan;
    getTangentAndBitangentAroundPoint(outward_normal, radius, phi, theta, tan, bitan);

    // store tangents and bitangents in the hit record if needed
    rec.tangent = tan;
    rec.bitangent = bitan;


    // compute UV coordinates
    get_sphere_uv(outward_normal, rec.u, rec.v, m_mapping);

    return true;
}

double sphere::pdf_value(const point3& o, const vector3& v) const
{
    // This method only works for stationary spheres.

    hit_record rec;
    if (!this->hit(ray(o, v), interval(SHADOW_ACNE_FIX, infinity), rec, 0))
        return 0;

    auto cos_theta_max = sqrt(1 - radius * radius / vector_length_squared(center1 - o));
    auto solid_angle = 2 * M_PI * (1 - cos_theta_max);

    return  1 / solid_angle;
}

/// <summary>
/// Random special implementation for sphere (override base)
/// </summary>
/// <param name="origin"></param>
/// <returns></returns>
vector3 sphere::random(const point3& o, randomizer2& rnd) const
{
    vector3 direction = center1 - o;
    auto distance_squared = vector_length_squared(direction);
    onb uvw;
    uvw.build_from_w(direction);
    return uvw.local(rnd.random_to_sphere(radius, distance_squared));
}



point3 sphere::sphere_center(double time) const
{
    // Linearly interpolate from center1 to center2 according to time, where t=0 yields
    // center1, and t=1 yields center2.
    return center1 + time * center_vec;
}

/// <summary>
/// https://medium.com/@dbildibay/ray-tracing-adventure-part-iv-678768947371
/// </summary>
/// <param name="p"></param>
/// <param name="radius"></param>
/// <param name="phi"></param>
/// <param name="theta"></param>
/// <param name="tan"></param>
/// <param name="bitan"></param>
void sphere::getTangentAndBitangentAroundPoint(const vector3& p, double radius, double phi, double theta, vector3& tan, vector3& bitan)
{
    // Tangent in the direction of increasing phi
    //tan.x = -p.z;
    //tan.y = 0;
    //tan.z = p.x;

    // hack FL to try having same normal rendering between sphere and obj
    tan.x = p.z;
    tan.y = 0;
    tan.z = -p.x;

    // Normalize the tangent
    tan = glm::normalize(tan);

    // Bitangent in the direction of increasing theta
    bitan = glm::cross(p, tan);

    // Ensure bitangent is perpendicular to both tangent and normal
    bitan = glm::normalize(bitan);
}


/// <summary>
/// Update the internal AABB of the mesh.
/// Warning: run this when the mesh is updated.
/// </summary>
void sphere::updateBoundingBox()
{
    // to implement
}