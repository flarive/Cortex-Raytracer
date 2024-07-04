#pragma once

//#include "../constants.h"
#include "../utilities/types.h"
#include "../utilities/randomizer.h"
#include "../utilities/randomizer2.h"
#include "../textures/image_texture.h"
#include "../onb.h"
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

    virtual double value(const vector3& direction) const = 0;
	virtual vector3 generate(randomizer2& rnd, scatter_record& rec) = 0;
};