#include "hittable.h"


double hittable::pdf_value(const point3& o, const vector3& v) const
{
    return 0.0;
}

vector3 hittable::random(const vector3& o) const
{
    return vector3(1, 0, 0);
}

void hittable::setName(std::string _name)
{
    name = _name;
}

std::string hittable::getName() const
{
    return(name);
}