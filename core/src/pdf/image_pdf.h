#pragma once

#include "pdf.h"
#include "../utilities/types.h"
#include "../utilities/randomizer.h"
#include "../textures/image_texture.h"
#include "../onb.h"
#include "../primitives/hittable.h"
#include "../misc/scatter_record.h"

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