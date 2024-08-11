#include "standard_output.h"

#include "../utilities/interval.h"


int standard_output::init_output(const size_t dataSize)
{
    return 0;
}

int standard_output::write_to_output(int x, int y, color pixel_color) const
{
    // Write the translated [0,255] value of each color component.
    // Static Variable gets constructed only once no matter how many times the function is called.
    static const interval intensity(0.000, 0.999);

    std::cout << "p " << x << " " << y << " "
        << static_cast<int>(256 * intensity.clamp(pixel_color.r())) << " "
        << static_cast<int>(256 * intensity.clamp(pixel_color.g())) << " "
        << static_cast<int>(256 * intensity.clamp(pixel_color.b())) << "\n";

    return 0;
}

int standard_output::clean_output()
{
    return 0;
}