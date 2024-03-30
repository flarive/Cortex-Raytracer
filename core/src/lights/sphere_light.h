#pragma once

#include "light.h"
#include "../primitives/hittable.h"
#include "../misc/color.h"
#include "../materials/diffuse_light.h"
#include "../utilities/math_utils.h"
#include "../singleton.h"

/// <summary>
/// Sphere light
/// </summary>
class sphere_light : public light
{
public:
    sphere_light(point3 _center, double _radius, double _intensity, color _color, string _name = "SphereLight", bool _invisible = true)
    {
        intensity = _intensity;
        c = _color * _intensity;
        invisible = _invisible;

        center1 = _center;
        radius = _radius;

        name = _name;

        mat = std::make_shared<diffuse_light>(c, false, invisible);

        
        // calculate stationary sphere bounding box for ray optimizations
        vector3 rvec = vector3(radius, radius, radius);
        bbox = aabb(center1 - rvec, center1 + rvec);
    }

    aabb bounding_box() const override;

    point3 getPosition() const override;


    /// <summary>
    /// Logic of sphere ray hit detection
    /// </summary>
    /// <param name="r"></param>
    /// <param name="ray_t"></param>
    /// <param name="rec"></param>
    /// <returns></returns>
    bool hit(const ray& r, interval ray_t, hit_record& rec, int depth) const override;
  

    double pdf_value(const point3& o, const vector3& v) const override;


    /// <summary>
    /// Random special implementation for sphere lights (override base)
    /// </summary>
    /// <param name="origin"></param>
    /// <returns></returns>
    vector3 random(const point3& o) const override;


private:
    point3 center1;
    double radius;
    vector3 center_vec;
};