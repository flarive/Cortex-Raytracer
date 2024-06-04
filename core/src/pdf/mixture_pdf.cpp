#include "mixture_pdf.h"


double mixture_pdf::value(const vector3& direction) const
{
	return proportion * (p[0]->value(direction)) + (1.0 - proportion) * (p[1]->value(direction));
}

vector3 mixture_pdf::generate(randomizer& rnd, scatter_record& rec)
{
	if (randomizer::random_double() < proportion)
	{
		return p[0]->generate(rnd, rec);
	}
	else
	{
		return p[1]->generate(rnd, rec);
	}
}