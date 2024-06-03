#include "pdf.h"

#include "misc/scatter_record.h"
#include "utilities/uvmapping.h"

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



double sphere_pdf::value(const vector3& direction) const
{
    return 1 / (4 * M_PI);
}

vector3 sphere_pdf::generate(randomizer& rnd, scatter_record& rec)
{
    return randomizer::random_unit_vector();
}




double hittable_pdf::value(const vector3& direction) const
{
    return objects.pdf_value(origin, direction);
}

vector3 hittable_pdf::generate(randomizer& rnd, scatter_record& rec)
{
    return objects.random(origin);
}




double mixture_pdf::value(const vector3& direction) const
{
    return 0.5 * p[0]->value(direction) + 0.5 * p[1]->value(direction);
}

vector3 mixture_pdf::generate(randomizer& rnd, scatter_record& rec)
{
    if (randomizer::random_double() < 0.5)
        return p[0]->generate(rnd, rec);
    else
        return p[1]->generate(rnd, rec);
}




double AnisotropicPhong_pdf::value(const vector3& direction) const
{
    const double cosine = vector_multiply_to_double(direction, m_onb.Normal());
    if (cosine < 0) return 0;

    return cosine * M_1_PI;
}

vector3 AnisotropicPhong_pdf::generate(randomizer& rnd, scatter_record& rec)
{
	double phase;
	bool flip;

	double xi = rnd.getZeroOne();
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

	xi = rnd.getZeroOne();
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

	if (rnd.getZeroOne() < diffuseProbability)
	{
		rec.attenuation = rec.diffuseColor;
		return m_onb.LocalToGlobal(rnd.getRandomCosineDirection());
	}

	// I don't like the white specular color that's typical in obj files, mix it with the diffuse color
	rec.attenuation = 0.8 * rec.specularColor + 0.2 * rec.diffuseColor;

	return GetSpecularReflected(h, kh);
}

void AnisotropicPhong_pdf::DealWithQuadrants(double& xi, double& phase, bool& flip)
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





image_pdf::image_pdf(std::shared_ptr<image_texture>& img) : image(img), m_width(img->getWidth()), m_height(img->getHeight()), m_channels(3), m_pData(img->getData())
{
	unsigned int k = 0;
	float angleFrac = M_PI / float(m_height);
	float theta = angleFrac * 0.5f;
	float sinTheta = 0.f;
	float* pSinTheta = (float*)alloca(sizeof(float) * m_height);
	for (unsigned int i = 0; i < m_height; i++, theta += angleFrac)
	{
		pSinTheta[i] = sin(theta);
	}
	// convert the data into a marginal CDF along the columns
	pBuffer = (float*)malloc(m_width * (m_height + 1) * sizeof(float));
	pUDist = &pBuffer[m_width * m_height];
	for (unsigned int i = 0, m = 0; i < m_width; i++, m += m_height)
	{
		float* pVDist = &pBuffer[m];
		k = i * m_channels;
		pVDist[0] = 0.2126f * m_pData[k + 0] + 0.7152f * m_pData[k + 1] + 0.0722f * m_pData[k + 2];
		pVDist[0] *= pSinTheta[0];
		for (unsigned int j = 1, k = (m_width + i) * m_channels; j < m_height; j++, k += m_width * m_channels)
		{
			float lum = 0.2126f * m_pData[k + 0] + 0.7152f * m_pData[k + 1] + 0.0722f * m_pData[k + 2];
			lum *= pSinTheta[j];
			pVDist[j] = pVDist[j - 1] + lum;

		}
		if (i == 0)
		{
			pUDist[i] = pVDist[m_height - 1];
		}
		else
		{
			pUDist[i] = pUDist[i - 1] + pVDist[m_height - 1];
		}
	}
}

double image_pdf::value(const vector3& direction) const
{
	double _u, _v; get_spherical_uv(randomizer::unit_vector(direction), _u, _v);
	_u = 1. - _u;
	int u = _u * double(m_height - 1), v = _v * double(m_width - 1);
	if (u < 0) u = 0;
	if (u >= m_height) u = m_height - 1;

	if (v < 0) v = 0;
	if (v >= m_width) v = m_width - 1;

	float angleFrac = M_PI / float(m_height);
	float invPdfNorm = (2.f * float(M_PI * M_PI)) / float(m_width * m_height);
	float* pVDist = &pBuffer[m_height * u];
	// compute the actual PDF
	float pdfU = (u == 0) ? (pUDist[0]) : (pUDist[u] - pUDist[u - 1]);
	pdfU /= pUDist[m_width - 1];
	float pdfV = (v == 0) ? (pVDist[0]) : (pVDist[v] - pVDist[v - 1]);
	pdfV /= pVDist[m_height - 1];
	float theta = angleFrac * 0.5 + angleFrac * u;
	float Pdf = (pdfU * pdfV) * sin(theta) / invPdfNorm;

	return Pdf;
}

vector3 image_pdf::generate(randomizer& rnd, scatter_record& rec)
{
	double r1 = randomizer::random_double(), r2 = randomizer::random_double();

	float maxUVal = pUDist[m_width - 1];
	float* pUPos = std::lower_bound(pUDist, pUDist + m_width,
		r1 * maxUVal);
	int u = pUPos - pUDist;
	float* pVDist = &pBuffer[m_height * u];
	float* pVPos = std::lower_bound(pVDist, pVDist + m_height,
		r2 * pVDist[m_height - 1]);
	int v = pVPos - pVDist;

	double _u = double(u) / m_height, _v = double(v) / m_width;
	_u = 1. - _u;

	return from_spherical_uv(_u, _v);
}




double mixture_pdf2::value(const vector3& direction) const
{
	return proportion * (p[0]->value(direction)) + (1.0 - proportion) * (p[1]->value(direction));
}

vector3 mixture_pdf2::generate(randomizer& rnd, scatter_record& rec)
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
