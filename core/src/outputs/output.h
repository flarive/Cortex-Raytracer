#pragma once

#include "../misc/color.h"

class output
{
public:
	virtual int init_output(const size_t dataSize);
	virtual int write_to_output(int x, int y, color pixel_color) const;
	virtual int clean_output();
};

