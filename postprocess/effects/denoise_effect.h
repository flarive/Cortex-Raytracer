#pragma once

#include "effect.h"

class denoise_effect : public effect
{
	GLuint apply(const std::string& inputPath, const std::string& outputPath, int width, int height, pmap params) override;
};
