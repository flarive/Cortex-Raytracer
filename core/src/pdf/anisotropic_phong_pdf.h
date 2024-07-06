#pragma once

#include "pdf.h"
#include "../utilities/types.h"
#include "../utilities/randomizer2.h"
#include "../textures/image_texture.h"
#include "../onb.h"
#include "../primitives/hittable.h"
#include "../misc/scatter_record.h"

class anisotropic_phong_pdf : public pdf
{
public:
	anisotropic_phong_pdf(const vector3& inc, const vector3& norm, double Nu, double Nv)
		: m_incident(inc), m_onb(norm, inc), m_nu(Nu), m_nv(Nv)
	{
		const double nu1 = m_nu + 1.;
		const double nv1 = m_nv + 1.;
		m_prefactor1 = sqrt(nu1 / nv1);
		m_prefactor2 = sqrt(nu1 * nv1) / (2. * M_PI);
	}

	double value(const vector3& direction) const override;
	vector3 generate(scatter_record& rec) override;

private:
	inline static double Schlick(const double val, double cosine)
	{
		return val + (1. - val) * pow(1. - cosine, 5.);
	}

	inline vector3 GetSpecularReflected(const vector3& h, double kh) const
	{
		return m_incident + 2. * kh * h;
	}

	inline double GetSpecularPDH(const vector3& h, double kh, double cos2, double sin2) const
	{
		return GetHPDH(h, cos2, sin2) / (4. * kh);
	}

	inline double GetHPDH(const vector3& h, double cos2, double sin2) const
	{
		auto kkk = m_onb.Normal();
		double nh = h.x * kkk.x + h.y * kkk.y + h.z * kkk.z;

		return m_prefactor2 * glm::pow(nh, m_nu * cos2 + m_nv * sin2);
	}

	static void DealWithQuadrants(double& xi, double& phase, bool& flip);

	vector3 m_incident;
	onb m_onb;

	double m_nu;
	double m_nv;

	double m_prefactor1;
	double m_prefactor2;
};