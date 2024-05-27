#include "spot_light.h"

#include "../constants.h"
#include "../materials/diffuse_spot_light.h"
#include "../textures/solid_color_texture.h"
#include "../utilities/math_utils.h"
#include "../utilities/randomizer.h"
#include "../utilities/uvmapping.h"
#include "../misc/singleton.h"
#include "../onb.h"

spot_light::spot_light(vector3 _direction, double _cosTotalWidth, double _cosFalloffStart, double _intensity, color _color, string _name, bool _invisible)
    : light(_direction, _intensity, _color, _invisible, _name)
{
    m_cosTotalWidth = _cosTotalWidth;
    m_cosFalloffStart = _cosFalloffStart;

    m_mat = std::make_shared<diffuse_spot_light>(std::make_shared<solid_color_texture>(_color), _direction, _cosTotalWidth, _cosFalloffStart, _intensity, m_invisible);

    // calculate stationary sphere bounding box for ray optimizations
    //vector3 rvec = vector3(radius, radius, radius);
    //m_bbox = aabb(m_position - rvec, m_position + rvec);
}

aabb spot_light::bounding_box() const
{
    return m_bbox;
}

bool spot_light::hit(const ray& r, interval ray_t, hit_record& rec, int depth) const
{
    return true;
}

double spot_light::pdf_value(const point3& o, const vector3& v) const
{
    return 0;
}

vector3 spot_light::random(const point3& o) const
{
    return vector3();
}