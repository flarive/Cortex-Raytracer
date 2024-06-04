#pragma once

#include "pdf.h"
#include "../utilities/types.h"
#include "../utilities/randomizer.h"
#include "../textures/image_texture.h"
#include "../onb.h"
#include "../primitives/hittable.h"
#include "../misc/scatter_record.h"

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
