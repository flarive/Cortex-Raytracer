#include "sphere.h"

#include "hittable_list.h"
#include "../lights/light.h"
#include "../constants.h"
#include "../singleton.h"
#include "../onb.h"

#include <glm/glm.hpp>

using std::shared_ptr;
using std::string;


sphere::sphere(point3 _center, double _radius, shared_ptr<material> _material, string _name)
    : center1(_center), radius(_radius), mat(_material), is_moving(false)
{
    name = _name;

    // calculate stationary sphere bounding box for ray optimizations
    vector3 rvec = vector3(radius, radius, radius);
    bbox = aabb(center1 - rvec, center1 + rvec);
}

sphere::sphere(point3 _center1, point3 _center2, double _radius, shared_ptr<material> _material, string _name)
    : center1(_center1), radius(_radius), mat(_material), is_moving(true)
{
    name = _name;

    // calculate moving sphere bounding box for ray optimizations
    vector3 rvec = vector3(radius, radius, radius);
    aabb box1(_center1 - rvec, _center1 + rvec);
    aabb box2(_center2 - rvec, _center2 + rvec);
    bbox = aabb(box1, box2);
    //bbox = aabb::surrounding(box1, box2);

    center_vec = _center2 - _center1;
}

aabb sphere::bounding_box() const
{
    return bbox;
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

    // alpha_texture ??????????????????????????????
    //Singleton* singleton = Singleton::getInstance();
    //if (singleton)
    //{
    //    auto renderParams = singleton->value();
    //    if (depth == renderParams.recursionMaxDepth)
    //    {
    //        return false;
    //    }
    //}



    // number of hits encountered by the ray (only the nearest ?)
    rec.t = root;

    // point coordinate of the hit
    rec.hit_point = r.at(rec.t);

    // material of the hit object
    rec.mat = mat;

    // name of the primitive hit by the ray
    rec.name = name;
    rec.bbox = bbox;

    // set normal and front-face tracking
    vector3 outward_normal = (rec.hit_point - center) / radius;
    rec.set_face_normal(r, outward_normal);

    // UV coordinates
    get_sphere_uv(outward_normal, rec.u, rec.v);

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
vector3 sphere::random(const point3& o) const
{
    vector3 direction = center1 - o;
    auto distance_squared = vector_length_squared(direction);
    onb uvw;
    uvw.build_from_w(direction);
    return uvw.local(random_to_sphere(radius, distance_squared));
}



point3 sphere::sphere_center(double time) const
{
    // Linearly interpolate from center1 to center2 according to time, where t=0 yields
    // center1, and t=1 yields center2.
    return center1 + time * center_vec;
}


/// <summary>
/// Update the internal AABB of the mesh.
/// Warning: run this when the mesh is updated.
/// </summary>
void sphere::updateBoundingBox()
{
    // to implement
}