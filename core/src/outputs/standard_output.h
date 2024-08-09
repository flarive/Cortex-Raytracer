#pragma once

#include "output.h"
#include "../misc/color.h"

class standard_output : public output
{
public:
    int init_output(const size_t dataSize) override;
    int write_to_output(int x, int y, color pixel_color) const override;
    int clean_output() override;
};