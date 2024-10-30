#include "hittable.h"

double hittable::pdf_value(const point3& o, const vector3& v, randomizer& rnd) const
{
    return 0.0;
}

vector3 hittable::random(const vector3& o, randomizer& rnd) const
{
    return vector3(1, 0, 0);
}

void hittable::setName(std::string _name)
{
    m_name = _name;
}

std::string hittable::getName() const
{
    return m_name;
}