#pragma once

#include "constants.h"
#include "utilities/types.h"
#include "utilities/randomizer.h"
#include "textures/image_texture.h"
#include "onb.h"
#include "primitives/hittable.h"

// avoid circular dependency
class scatter_record;


/// <summary>
/// Probability Distribution Function (henceforth PDF).
/// In short, a PDF is a continuous function that can be integrated over to determine how likely a result is over an integral.
/// Remember that the PDF is a probability function.
/// </summary>
class pdf
{
public:
	virtual ~pdf() {}

    virtual double value(const vector3& direction) const = 0;
	virtual vector3 generate(randomizer& rnd, scatter_record& rec) = 0;
};


class cosine_pdf : public pdf
{
public:
    cosine_pdf(const vector3& w) { uvw.build_from_w(w); }

    double value(const vector3& direction) const override;
	vector3 generate(randomizer& rnd, scatter_record& rec) override;


private:
    onb uvw;
};


class sphere_pdf : public pdf
{
public:
    sphere_pdf() { }

    double value(const vector3& direction) const override;
	vector3 generate(randomizer& rnd, scatter_record& rec) override;
};


class hittable_pdf : public pdf
{
public:
    hittable_pdf(const hittable& _objects, const point3& _origin)
        : objects(_objects), origin(_origin)
    {}

    double value(const vector3& direction) const override;
	vector3 generate(randomizer& rnd, scatter_record& rec) override;


private:
    const hittable& objects;
    point3 origin;
};



///https://github.com/Drummersbrother/raytracing-in-one-weekend/blob/90b1d3d7ce7f6f9244bcb925c77baed4e9d51705/pdf.h
class mixture_pdf : public pdf
{
public:
	mixture_pdf() : proportion(0.5) { p[0] = nullptr; p[1] = nullptr; }
	mixture_pdf(std::shared_ptr<pdf> p0, std::shared_ptr<pdf> p1) : proportion(0.5) { p[0] = p0; p[1] = p1; }
	mixture_pdf(std::shared_ptr<pdf> p0, std::shared_ptr<pdf> p1, double prop) : proportion(prop) { p[0] = p0; p[1] = p1; }

	double value(const vector3& direction) const override;
	vector3 generate(randomizer& rnd, scatter_record& rec) override;

public:
	double proportion = 0.0;
	std::shared_ptr<pdf> p[2];
};



// https://github.com/Drummersbrother/raytracing-in-one-weekend/blob/90b1d3d7ce7f6f9244bcb925c77baed4e9d51705/main.cpp#L26
class image_pdf : public pdf
{
public:
	image_pdf(std::shared_ptr<image_texture>& img);
	
	double value(const vector3& direction) const override;
	vector3 generate(randomizer& rnd, scatter_record& rec) override;

public:
	std::shared_ptr<image_texture> m_image;
	int m_width, m_height, m_channels;
	float* pUDist, * pBuffer;
	float* m_pData;
};


class AnisotropicPhong_pdf : public pdf
{
public:
	AnisotropicPhong_pdf(const vector3& inc, const vector3& norm, double Nu, double Nv)
		: m_incident(inc), m_onb(norm, inc), m_nu(Nu), m_nv(Nv)
	{
		const double nu1 = m_nu + 1.;
		const double nv1 = m_nv + 1.;
		m_prefactor1 = sqrt(nu1 / nv1);
		m_prefactor2 = sqrt(nu1 * nv1) / (2. * M_PI);
	}

	double value(const vector3& direction) const override;
	vector3 generate(randomizer& rnd, scatter_record& rec) override;

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