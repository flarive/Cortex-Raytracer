#pragma once

#include "../misc/color.h"

#define WIN32_LEAN_AND_MEAN // Exclude rarely-used services from Windows headers
#define NOMINMAX // Prevent the definition of min and max macros
#include <windows.h>

class output
{
public:
	virtual int init_output(const size_t dataSize);
	virtual int write_to_output(int x, int y, color pixel_color) const;
	virtual int clean_output();
};