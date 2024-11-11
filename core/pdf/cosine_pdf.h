#pragma once

#include "pdf.h"
#include "../utilities/types.h"
#include "../randomizers/randomize.h"
#include "../textures/image_texture.h"
#include "../misc/onb.h"
#include "../primitives/hittable.h"
#include "../misc/scatter_record.h"

class cosine_pdf : public pdf
{
public:
    cosine_pdf(const vector3& w) { uvw.build_from_w(w); }

    double value(const vector3& direction, randomizer& rnd) const override;
    vector3 generate(scatter_record& rec, randomizer& rnd) override;


private:
    onb uvw;
};
