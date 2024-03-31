#pragma once

// Disable strict warnings for this header from the Microsoft Visual C++ compiler.
#ifdef _MSC_VER
#pragma warning (push, 0)
#endif



#include <cstdlib>
#include <iostream>

class bitmap_image
{
public:
    bitmap_image();
    bitmap_image(const char* image_filename);
    
    ~bitmap_image();



    bool load(const std::string filename);

    int width()  const;
    int height() const;

    const unsigned char* pixel_data(int x, int y) const;


private:
    const int bytes_per_pixel = 3;
    unsigned char* data = NULL;
    int image_width = 0;
    int image_height = 0;
    int bytes_per_scanline = 0;

    static int clamp(int x, int low, int high)
    {
        // Return the value clamped to the range [low, high).
        if (x < low) return low;
        if (x < high) return x;
        return high - 1;
    }
};

// Restore MSVC compiler warnings
#ifdef _MSC_VER
#pragma warning (pop)
#endif