#include "bitmap_image.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include "stb/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"



bitmap_image::bitmap_image() : data(nullptr)
{
}

bitmap_image::bitmap_image(const char* image_filename)
{
    // Loads image data from the specified file. If the RTW_IMAGES environment variable is
    // defined, looks only in that directory for the image file. If the image was not found,
    // searches for the specified image file first from the current directory, then in the
    // images/ subdirectory, then the _parent's_ images/ subdirectory, and then _that_
    // parent, on so on, for six levels up. If the image was not loaded successfully,
    // width() and height() will return 0.

    auto filename = std::string(image_filename);
    auto imagedir = getenv("RTW_IMAGES");

    // Hunt for the image file in some likely locations.
    if (imagedir && load(std::string(imagedir) + "/" + image_filename)) return;
    if (load(filename)) return;

    std::cerr << "ERROR: Could not load image file '" << image_filename << "'.\n";
}

bitmap_image::~bitmap_image()
{
    STBI_FREE(data);
}

bool bitmap_image::load(const std::string filename)
{
    // Loads image data from the given file name. Returns true if the load succeeded.
    auto n = bytes_per_pixel; // Dummy out parameter: original components per pixel
    data = stbi_load(filename.c_str(), &image_width, &image_height, &n, bytes_per_pixel);
    bytes_per_scanline = image_width * bytes_per_pixel;
    return data != nullptr;
}

int bitmap_image::width()  const
{
    return (data == nullptr) ? 0 : image_width;
}

int bitmap_image::height() const
{
    return (data == nullptr) ? 0 : image_height;
}

const unsigned char* bitmap_image::pixel_data(int x, int y) const
{
    // Return the address of the three bytes of the pixel at x,y (or magenta if no data).
    static unsigned char magenta[] = { 255, 0, 255 };
    if (data == nullptr) return magenta;

    x = clamp(x, 0, image_width);
    y = clamp(y, 0, image_height);

    return data + y * bytes_per_scanline + x * bytes_per_pixel;
}


uint8_t* bitmap_image::buildPNG(std::vector<std::vector<color>> image, const int width, const int height, const int samples_per_pixel, bool gamma_correction)
{
    constexpr int CHANNEL_NUM = 4; // indexed (really 1 or 0)

    /*** NOTICE!! You have to use uint8_t array to pass in stb function  ***/
    // Because the size of color is normally 255, 8bit.
    // If you don't use this one, you will get a weird imge.
    uint8_t* pixels = new uint8_t[width * height * CHANNEL_NUM];

    int index = 0;
    for (int j = 0; j < height; ++j)
    {
        for (int i = 0; i < width; ++i)
        {
            color pixel_color = image[j][i];

            double r = pixel_color.r();
            double g = pixel_color.g();
            double b = pixel_color.b();

            // Replace NaN components with zero.
            if (r != r) r = 0.0;
            if (g != g) g = 0.0;
            if (b != b) b = 0.0;

            // Anti aliasing
            // Divide the color by the number of samples.
            if (samples_per_pixel > 0)
            {
                double scale = 1.0 / samples_per_pixel;
                r *= scale;
                g *= scale;
                b *= scale;
            }

            // Gamma correction
            // Apply the linear to gamma transform
            // Helps to have a much more consistent ramp from darkness to lightness in the final image
            if (gamma_correction)
            {
                r = color::linear_to_gamma(r);
                g = color::linear_to_gamma(g);
                b = color::linear_to_gamma(b);

                //r = 0.5 * r;
                //g = 0.5 * g;
                //b = 0.5 * b;
            }

            // Write the translated [0,255] value of each color component.
            // Static Variable gets constructed only once no matter how many times the function is called.
            static const interval intensity(0.000, 0.999);

            pixels[index++] = static_cast<int>(256 * intensity.clamp(r));
            pixels[index++] = static_cast<int>(256 * intensity.clamp(g));
            pixels[index++] = static_cast<int>(256 * intensity.clamp(b));
        }
    }

    return pixels;
}

bool bitmap_image::saveAsPNG(const std::string& filename, int width, int height, int comp, const uint8_t* data, int strides_per_byte)
{
    int res = stbi_write_png(filename.c_str(), width, height, comp, data, strides_per_byte);

    return res == 1;
}