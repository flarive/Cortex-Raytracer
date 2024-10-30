#include "cosine_pdf.h"
#include "../misc/singleton.h"

#include <glm/glm.hpp>

double cosine_pdf::value(const vector3& direction, randomizer& rnd) const
{
    auto cosine_theta = dot(unit_vector(direction), uvw.w());
    return fmax(0, cosine_theta / M_PI);
}

vector3 cosine_pdf::generate(scatter_record& rec, randomizer& rnd)
{
    return uvw.local(rnd.get_cosine_direction());
}