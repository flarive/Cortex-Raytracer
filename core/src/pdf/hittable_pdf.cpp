#include "hittable_pdf.h"

#include <glm/glm.hpp>

double hittable_pdf::value(const vector3& direction) const
{
    return objects.pdf_value(origin, direction);
}

vector3 hittable_pdf::generate(randomizer2& rnd, scatter_record& rec)
{
    return objects.random(origin, rnd);
}
