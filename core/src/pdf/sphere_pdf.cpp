#include "sphere_pdf.h"

double sphere_pdf::value(const vector3& direction) const
{
    return 1 / (4 * M_PI);
}

vector3 sphere_pdf::generate(randomizer& rnd, scatter_record& rec)
{
    return randomizer::random_unit_vector();
}