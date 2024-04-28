#include "light.h"

light::light(point3 _position, double _intensity, color _color, bool _invisible, std::string _name)
    : m_position(_position), m_intensity(_intensity), m_color(_color), m_invisible(_invisible)
{
    m_name = _name;
}

//aabb light::bounding_box() const
//{
//    return m_bbox;
//}

double light::getIntensity() const
{
    return m_intensity;
}

color light::getColor() const
{
    return m_color;
}

point3 light::getPosition() const
{
    return m_position;
}

void light::updateBoundingBox()
{
    // to implement
}