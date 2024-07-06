#include "image_pdf.h"

#include "../misc/singleton.h"

image_pdf::image_pdf(std::shared_ptr<image_texture>& img)
	: m_image(img), m_width(img->getWidth()), m_height(img->getHeight()), m_channels(3), m_pData(img->get_data_float())
{
	unsigned int k = 0;
	float angleFrac = M_PI / float(m_height);
	float theta = angleFrac * 0.5f;
	float sinTheta = 0.f;
	float* pSinTheta = (float*)malloc(sizeof(float) * m_height);
	for (unsigned int i = 0; i < m_height; i++, theta += angleFrac)
	{
		pSinTheta[i] = sin(theta);
	}
	// convert the data into a marginal CDF along the columns
	m_pBuffer = (float*)malloc(m_width * (m_height + 1) * sizeof(float));
	m_pUDist = &m_pBuffer[m_width * m_height];
	for (unsigned int i = 0, m = 0; i < m_width; i++, m += m_height)
	{
		float* pVDist = &m_pBuffer[m];
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
			m_pUDist[i] = pVDist[m_height - 1];
		}
		else
		{
			m_pUDist[i] = m_pUDist[i - 1] + pVDist[m_height - 1];
		}
	}
}

double image_pdf::value(const vector3& direction) const
{
	double _u, _v; get_spherical_uv(unit_vector(direction), _u, _v);
	_u = 1. - _u;
	int u = _u * double(m_height - 1), v = _v * double(m_width - 1);
	if (u < 0) u = 0;
	if (u >= m_height) u = m_height - 1;

	if (v < 0) v = 0;
	if (v >= m_width) v = m_width - 1;

	float angleFrac = M_PI / float(m_height);
	float invPdfNorm = (2.f * float(M_PI * M_PI)) / float(m_width * m_height);
	float* pVDist = &m_pBuffer[m_height * u];
	// compute the actual PDF
	float pdfU = (u == 0) ? (m_pUDist[0]) : (m_pUDist[u] - m_pUDist[u - 1]);
	pdfU /= m_pUDist[m_width - 1];
	float pdfV = (v == 0) ? (pVDist[0]) : (pVDist[v] - pVDist[v - 1]);
	pdfV /= pVDist[m_height - 1];
	float theta = angleFrac * 0.5 + angleFrac * u;
	float Pdf = (pdfU * pdfV) * sin(theta) / invPdfNorm;

	return Pdf;
}

vector3 image_pdf::generate(scatter_record& rec)
{
	double r1 = Singleton::getInstance()->rnd().get_real(0.0, 1.0);
	double r2 = Singleton::getInstance()->rnd().get_real(0.0, 1.0);

	float maxUVal = m_pUDist[m_width - 1];
	float* pUPos = std::lower_bound(m_pUDist, m_pUDist + m_width, r1 * maxUVal);
	int u = pUPos - m_pUDist;
	float* pVDist = &m_pBuffer[m_height * u];
	float* pVPos = std::lower_bound(pVDist, pVDist + m_height, r2 * pVDist[m_height - 1]);
	int v = pVPos - pVDist;

	double _u = double(u) / m_height, _v = double(v) / m_width;
	_u = 1. - _u;

	return from_spherical_uv(_u, _v);
}