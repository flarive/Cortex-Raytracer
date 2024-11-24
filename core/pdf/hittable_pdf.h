#pragma once

#include "pdf.h"
#include "../utilities/types.h"
#include "../randomizers/randomizer.h"
#include "../textures/image_texture.h"
#include "../misc/onb.h"
#include "../primitives/hittable.h"
#include "../misc/scatter_record.h"

class hittable_pdf : public pdf
{
public:
    hittable_pdf(const hittable& _objects, const point3& _origin)
        : objects(_objects), origin(_origin)
    {}

    double value(const vector3& direction, randomizer& rnd) const override;
    vector3 generate(scatter_record& rec, randomizer& rnd) override;


private:
    const hittable& objects;
    point3 origin;
};