#include "onb.h"

void onb::build_from_w(const vector3& n)
{
    axis[2] = unit_vector(n);
    vector3 a = (std::fabs(w().x) > 0.9) ? vector3(0, 1, 0) : vector3(1, 0, 0);
    axis[1] = unit_vector(glm::cross(w(), a));
    axis[0] = glm::cross(w(), v());
}