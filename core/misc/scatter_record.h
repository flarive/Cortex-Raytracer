#pragma once

#include "color.h"
#include "ray.h"
#include "../pdf/pdf.h"

class scatter_record
{
public:
	color attenuation{};
	std::shared_ptr<pdf> pdf_ptr = nullptr;
	bool skip_pdf = false; // is specular
	ray skip_pdf_ray; // specular_ray

	color diffuseColor{};  // used only by AnisotropicPhong
	color specularColor{}; // used only by AnisotropicPhong

	double alpha_value = 1.0; // If no alpha texture, return 1.0 (fully opaque)
};