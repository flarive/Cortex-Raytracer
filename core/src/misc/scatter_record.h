#pragma once

#include "color.h"
#include "ray.h"
#include "../pdf.h"

class scatter_record
{
public:
	color attenuation;
	std::shared_ptr<pdf> pdf_ptr;
	bool skip_pdf; // is specular
	ray skip_pdf_ray; // specular_ray
};