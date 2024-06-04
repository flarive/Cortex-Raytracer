#include "cosine_pdf.h"

#include <glm/glm.hpp>

double cosine_pdf::value(const vector3& direction) const
{
    auto cosine_theta = dot(randomizer::unit_vector(direction), uvw.w());
    return fmax(0, cosine_theta / M_PI);
}

vector3 cosine_pdf::generate(randomizer& rnd, scatter_record& rec)
{
    return uvw.local(randomizer::random_cosine_direction());
}