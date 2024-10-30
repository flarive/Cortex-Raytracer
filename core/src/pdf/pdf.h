#pragma once

#include "../utilities/types.h"
#include "../utilities/randomizer.h"
#include "../textures/image_texture.h"
#include "../misc/onb.h"
#include "../primitives/hittable.h"

// avoid circular dependency
class scatter_record;


/// <summary>
/// Probability Distribution Function (henceforth PDF).
/// In short, a PDF is a continuous function that can be integrated over to determine how likely a result is over an integral.
/// Remember that the PDF is a probability function.
/// </summary>
class pdf
{
public:
	virtual ~pdf() {}

    virtual double value(const vector3& direction, randomizer& rnd) const = 0;
	virtual vector3 generate(scatter_record& rec, randomizer& rnd) = 0;
};