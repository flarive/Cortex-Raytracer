#pragma once

// Disable strict warnings for this header from the Microsoft Visual C++ compiler.
#ifdef _MSC_VER
#pragma warning (push, 0)
#endif

#include "../misc/color.h"

#include <vector>

class bitmap_image
{
public:
    bitmap_image();
    bitmap_image(std::string filepath);
    bitmap_image(unsigned char* imageData, size_t imageSize);
    
    ~bitmap_image();



    bool load(const std::string filepath);

    int width()  const;
    int height() const;
    int channels() const;

    unsigned char* get_data() const;
    float* get_data_float() const;

    const unsigned char* pixel_data(int x, int y) const;

    static uint8_t* buildPNG(std::vector<std::vector<color>> pixels, const int width, const int height, const int samples_per_pixel, bool gamma_correction);
    static bool saveAsPNG(const std::string& filename, int width, int height, int comp, const uint8_t* data, int strides_per_byte);

private:
    const int bytes_per_pixel = 3;
    unsigned char* data = NULL;
    int image_width = 0;
    int image_height = 0;
    int image_channels = 0;
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