#pragma once

#include "light.h"
#include "../misc/color.h"
#include "../utilities/types.h"
#include "../randomizers/randomizer.h"

/// <summary>
/// Omni directional light
/// </summary>
class omni_light : public light
{
public:
    omni_light(point3 _position, double _radius, double _intensity, color _color, std::string _name = "SphereLight", bool _invisible = true);


    aabb bounding_box() const override;

    /// <summary>
    /// Logic of sphere ray hit detection
    /// </summary>
    /// <param name="r"></param>
    /// <param name="ray_t"></param>
    /// <param name="rec"></param>
    /// <returns></returns>
    bool hit(const ray& r, interval ray_t, hit_record& rec, int depth, randomizer& rnd) const override;
  

    double pdf_value(const point3& o, const vector3& v, randomizer& rnd) const override;


    /// <summary>
    /// Random special implementation for sphere lights (override base)
    /// </summary>
    /// <param name="origin"></param>
    /// <returns></returns>
    vector3 random(const point3& o, randomizer& rnd) const override;


private:
    double radius = 0.0;
    vector3 center_vec{};
};