#include "sphere_pdf.h"

#include "../misc/singleton.h"

double sphere_pdf::value(const vector3& direction) const
{
    return 1 / (4 * M_PI);
}

vector3 sphere_pdf::generate(scatter_record& rec, randomizer& rnd)
{
    return rnd.get_unit_vector();
}