#include "pdf.h"

#include <glm/glm.hpp>


double cosine_pdf::value(const vector3& direction) const
{
    auto cosine_theta = dot(unit_vector(direction), uvw.w());
    return fmax(0, cosine_theta / M_PI);
}

vector3 cosine_pdf::generate() const
{
    return uvw.local(random_cosine_direction());
}



double sphere_pdf::value(const vector3& direction) const
{
    return 1 / (4 * M_PI);
}

vector3 sphere_pdf::generate() const
{
    return random_unit_vector();
}




double hittable_pdf::value(const vector3& direction) const
{
    return objects.pdf_value(origin, direction);
}

vector3 hittable_pdf::generate() const
{
    return objects.random(origin);
}




double mixture_pdf::value(const vector3& direction) const
{
    return 0.5 * p[0]->value(direction) + 0.5 * p[1]->value(direction);
}

vector3 mixture_pdf::generate() const
{
    if (random_double() < 0.5)
        return p[0]->generate();
    else
        return p[1]->generate();
}