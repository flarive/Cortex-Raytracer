#include "mixture_pdf.h"

#include "../misc/singleton.h"


double mixture_pdf::value(const vector3& direction, randomizer& rnd) const
{
	return proportion * (p[0]->value(direction, rnd)) + (1.0 - proportion) * (p[1]->value(direction, rnd));
}

vector3 mixture_pdf::generate(scatter_record& rec, randomizer& rnd)
{
	if (rnd.get_real(0.0, 1.0) < proportion)
	{
		return p[0]->generate(rec, rnd);
	}
	else
	{
		return p[1]->generate(rec, rnd);
	}
}