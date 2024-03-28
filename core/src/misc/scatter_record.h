#ifndef SCATTER_RECORD_H
#define SCATTER_RECORD_H

#include "color.h"
#include "../pdf.h"

class scatter_record
{
public:
	color attenuation;
	std::shared_ptr<pdf> pdf_ptr;
	bool skip_pdf; // means specular
	ray skip_pdf_ray;
};

#endif