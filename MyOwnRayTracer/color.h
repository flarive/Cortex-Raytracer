#ifndef COLOR_H
#define COLOR_H

#include "vec3.h"
#include "interval.h"

#include <iostream>

using color = vec3;

/// <summary>
/// Write pixel color to the output stream
/// </summary>
/// <param name="out"></param>
/// <param name="pixel_color"></param>
void write_color(std::ostream& out, color pixel_color)
{
    // Write the translated [0,255] value of each color component.
    out << static_cast<int>(255.999 * pixel_color.x()) << ' '
        << static_cast<int>(255.999 * pixel_color.y()) << ' '
        << static_cast<int>(255.999 * pixel_color.z()) << '\n';
}


/// <summary>
/// Write pixel color to the output stream with pixel sampling (antialiasing)
/// </summary>
/// <param name="out"></param>
/// <param name="pixel_color"></param>
/// <param name="samples_per_pixel"></param>
void write_color(std::ostream& out, color pixel_color, int samples_per_pixel)
{
    double r = pixel_color.x();
    double g = pixel_color.y();
    double b = pixel_color.z();

    // Divide the color by the number of samples.
    double scale = 1.0 / samples_per_pixel;
    r *= scale;
    g *= scale;
    b *= scale;

    // Write the translated [0,255] value of each color component.
    // Static Variable gets constructed only once no matter how many times the function is called.
    static const interval intensity(0.000, 0.999);

    out << static_cast<int>(256 * intensity.clamp(r)) << ' '
        << static_cast<int>(256 * intensity.clamp(g)) << ' '
        << static_cast<int>(256 * intensity.clamp(b)) << '\n';
}


#endif