#include "bitmap_image.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include "stb/stb_image.h"

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



