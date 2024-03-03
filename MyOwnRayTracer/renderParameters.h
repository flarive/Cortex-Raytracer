#pragma once

class renderParameters
{
public:

	bool quietMode = false;

	float ratio = 16.0 / 9.0;
	unsigned int width = 256;
	unsigned int height = static_cast<unsigned int>(width / ratio);
	
};