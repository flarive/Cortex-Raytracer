#include "anisotropic_phong_pdf.h"

double anisotropic_phong_pdf::value(const vector3& direction) const
{
	const double cosine = vector_multiply_to_double(direction, m_onb.Normal());
	if (cosine < 0) return 0;

	return cosine * M_1_PI;
}

vector3 anisotropic_phong_pdf::generate(randomizer2& rnd, scatter_record& rec)
{
	double phase;
	bool flip;

	double xi = rnd.get_real();
	DealWithQuadrants(xi, phase, flip);

	double phi = atan(m_prefactor1 * tan(M_PI_2 * xi));
	if (flip)
		phi = phase - phi;
	else
		phi += phase;

	const double c = cos(phi);
	const double s = sin(phi);
	const double c2 = c * c;
	const double s2 = 1. - c2;

	xi = rnd.get_real();
	DealWithQuadrants(xi, phase, flip);

	double theta = acos(pow(1. - xi, 1. / (m_nu * c2 + m_nv * s2 + 1.)));
	if (flip)
		theta = phase - theta;
	else
		theta += phase;

	const double st = sin(theta);
	const double ct = cos(theta);

	const double cos2 = ct * ct;
	const double sin2 = st * st;

	const vector3 h = m_onb.LocalToGlobal(vector3(st * c, st * s, ct));

	double diffuseProbability;
	double kh = 0.; // avoid complains about not being initialized

	if (vector_multiply_to_double(h, m_onb.Normal()) < 0)
		diffuseProbability = 1.;
	else
	{
		kh = vector_multiply_to_double(-m_incident, h);
		const double specularProbability = GetSpecularPDH(h, kh, cos2, sin2);
		const double weight = 1. + specularProbability;

		diffuseProbability = 1. / weight;
	}

	if (rnd.get_real() < diffuseProbability)
	{
		rec.attenuation = rec.diffuseColor;
		return m_onb.LocalToGlobal(rnd.get_cosine_direction());
	}

	// I don't like the white specular color that's typical in obj files, mix it with the diffuse color
	rec.attenuation = 0.8 * rec.specularColor + 0.2 * rec.diffuseColor;

	return GetSpecularReflected(h, kh);
}

void anisotropic_phong_pdf::DealWithQuadrants(double& xi, double& phase, bool& flip)
{
	phase = 0;
	flip = false;

	if (xi < 0.25)
	{
		xi *= 4;
	}
	else if (xi < 0.5)
	{
		xi = 1. - 4. * (0.5 - xi);
		phase = M_PI;
		flip = true;
	}
	else if (xi < 0.75)
	{
		xi = 1. - 4. * (0.75 - xi);
		phase = M_PI;
	}
	else
	{
		xi = 1. - 4. * (1. - xi);
		phase = 2. * M_PI;
		flip = true;
	}
}