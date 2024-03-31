#include "onb.h"

onb::onb()
{

}

vector3 onb::operator[](int i) const
{
    return axis[i];
}

vector3 onb::u() const
{
    return axis[0];
}

vector3 onb::v() const
{
    return axis[1];
}

vector3 onb::w() const
{
    return axis[2];
}

vector3 onb::local(double a, double b, double c) const
{
    return a * u() + b * v() + c * w();
}

vector3 onb::local(const vector3& a) const
{
    return a.x * u() + a.y * v() + a.z * w();
}

void onb::build_from_w(const vector3& n)
{
    axis[2] = unit_vector(n);
    vector3 a = (std::fabs(w().x) > 0.9) ? vector3(0, 1, 0) : vector3(1, 0, 0);
    axis[1] = unit_vector(glm::cross(w(), a));
    axis[0] = glm::cross(w(), v());
}