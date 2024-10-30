#include "hittable_pdf.h"

#include <glm/glm.hpp>

double hittable_pdf::value(const vector3& direction, randomizer& rnd) const
{
    return objects.pdf_value(origin, direction, rnd);
}

vector3 hittable_pdf::generate(scatter_record& rec, randomizer& rnd)
{
    return objects.random(origin, rnd);
}
