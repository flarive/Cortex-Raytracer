#pragma once

#include "pdf.h"
#include "../utilities/types.h"
#include "../utilities/randomizer.h"
#include "../textures/image_texture.h"
#include "../misc/onb.h"
#include "../primitives/hittable.h"
#include "../misc/scatter_record.h"

class sphere_pdf : public pdf
{
public:
    sphere_pdf() { }

    double value(const vector3& direction) const override;
    vector3 generate(scatter_record& rec) override;
};
