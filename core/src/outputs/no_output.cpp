#include "no_output.h"

#include "../utilities/interval.h"

int no_output::init_output(const size_t dataSize)
{
    return 0;
}

int no_output::write_to_output(int x, int y, color pixel_color) const
{
    return 0;
}

int no_output::clean_output()
{
    return 0;
}