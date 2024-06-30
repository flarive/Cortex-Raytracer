#pragma once

#include "light.h"
#include "../misc/color.h"
#include "../utilities/types.h"

/// <summary>
/// Directional light
/// </summary>
class directional_light : public light
{
public:
    directional_light(const point3& _position, const vector3& _u, const vector3& _v, double _intensity, color _color, std::string _name = "QuadLight", bool _invisible = true);
    
    void set_bounding_box();
    aabb bounding_box() const override;
    bool hit(const ray& r, interval ray_t, hit_record& rec, int depth) const override;
    bool is_interior(double a, double b, hit_record& rec) const;
    double pdf_value(const point3& origin, const vector3& v) const override;

    /// <summary>
    /// Random special implementation for quad light (override base)
    /// </summary>
    /// <param name="origin"></param>
    /// <returns></returns>
    vector3 random(const point3& origin) const override;


private:
    vector3 m_u{}; // vector representing the first side of the quadrilateral
    vector3 m_v{}; //  vector representing the second side of the quadrilateral
    vector3 m_normal{}; // vector representing quadrilateral normal
    double D = 0.0;
    vector3 w{}; // vector w is constant for a given quadrilateral, so we'll cache that value
    double area = 0.0;
};