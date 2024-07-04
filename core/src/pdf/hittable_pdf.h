#pragma once

#include "pdf.h"
#include "../utilities/types.h"
#include "../utilities/randomizer.h"
#include "../utilities/randomizer2.h"
#include "../textures/image_texture.h"
#include "../onb.h"
#include "../primitives/hittable.h"
#include "../misc/scatter_record.h"

class hittable_pdf : public pdf
{
public:
    hittable_pdf(const hittable& _objects, const point3& _origin)
        : objects(_objects), origin(_origin)
    {}

    double value(const vector3& direction) const override;
    vector3 generate(randomizer2& rnd, scatter_record& rec) override;


private:
    const hittable& objects;
    point3 origin;
};