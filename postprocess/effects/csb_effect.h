#pragma once

#include "effect.h"

class csb_effect : public effect
{
	GLuint apply(const std::string& inputPath, const std::string& outputPath, int width, int height, float radius) override;
};
