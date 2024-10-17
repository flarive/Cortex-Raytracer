#include "OpenImageDenoise/oidn.hpp"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include "stb/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

#include "parameters.h"

#include <iostream>
#include <vector>
#include <cmath>

// Function to apply inverse gamma correction (sRGB to linear)
float srgbToLinear(float srgbValue)
{
    if (srgbValue <= 0.04045f) {
        return srgbValue / 12.92f;
    }
    else {
        return std::pow((srgbValue + 0.055f) / 1.055f, 2.4f);
    }
}

// Convert an image from sRGB to linear space
void convertImageToLinear(std::vector<float>& image, int width, int height, int depth)
{
    for (int i = 0; i < width * height * depth; i++) {
        image[i] = srgbToLinear(image[i]);
    }
}

int main(int argc, char* argv[])
{
    parameters params = parameters::getArgs(argc, argv);
    
    int depth = 3; // Force 3 channels (RGB)

    int width, height, channels;
    // Load the image as floating-point data (STBI loads as float in the range [0, 1])
    unsigned char* imageData = stbi_load(params.inputpath.c_str(), &width, &height, &channels, depth);

    if (!imageData)
    {
        std::cerr << "Failed to load image: " << stbi_failure_reason() << std::endl;
        return -1;
    }

    std::cout << "Image loaded successfully: width = " << width << ", height = " << height << ", channels = " << channels << std::endl;

    // Create a vector to hold the image data
    std::vector<unsigned char> imageVector(imageData, imageData + (width * height * depth));

    // Free the original image data since we now have it in the vector
    stbi_image_free(imageData);

    // Apply inverse gamma correction (sRGB to linear)
    //convertImageToLinear(imageVector, width, height, depth);

    // Create an Open Image Denoise device
    oidn::DeviceRef device = oidn::newDevice(oidn::DeviceType::CPU); // CPU or GPU if available
    device.commit();

    // Check buffer size calculation
    size_t bufferSize = width * height * depth * sizeof(float);
    std::cout << "Allocating buffer of size: " << bufferSize << " bytes" << std::endl;

    // Create buffers for input/output images accessible by both host (CPU) and device (CPU/GPU)
    oidn::BufferRef colorBuf = device.newBuffer(bufferSize);

    // Check if buffer is properly allocated
    float* colorPtr = (float*)colorBuf.getData();
    if (colorPtr == nullptr)
    {
        std::cerr << "Error: colorPtr is null, buffer allocation failed!" << std::endl;
        return -1;
    }

    // Fill the input image buffer with the image data (already in float)
    for (int i = 0; i < width * height * depth; i++)
    {
        colorPtr[i] = imageVector[i] / 255.0f; // Normalize 8-bit data to the [0, 1] range
    }

    std::cout << "Buffer filled successfully." << std::endl;

    // Create a filter for denoising
    oidn::FilterRef filter = device.newFilter("RT"); // generic ray tracing filter
    filter.setImage("color", colorBuf, oidn::Format::Float3, width, height); // noisy input image
    filter.setImage("output", colorBuf, oidn::Format::Float3, width, height); // denoised output image
    filter.set("hdr", params.hdr); // input image is HDR
    filter.commit();

    // Perform denoising
    filter.execute();

    // Check for errors
    const char* errorMessage;
    if (device.getError(errorMessage) != oidn::Error::None)
    {
        std::cout << "Error: " << errorMessage << std::endl;
    }

    std::cout << "Denoising completed successfully." << std::endl;

    // Save the denoised image as PNG
    // First, convert the float data to unsigned char by normalizing to the range [0, 255]
    unsigned char* outputImage = new unsigned char[width * height * depth];

    for (int i = 0; i < width * height * depth; i++)
    {
        // Clamp values between 0 and 1, then scale to 255
        outputImage[i] = static_cast<unsigned char>(std::min(1.0f, std::max(0.0f, colorPtr[i])) * 255.0f);
    }

    // Save the image as PNG
    int result = stbi_write_png(params.outputpath.c_str(),
        width, height, depth,
        outputImage, width * depth);

    if (result)
        std::cout << "Image saved successfully." << std::endl;
    else
        std::cerr << "Failed to save image." << std::endl;

    // Clean up
    delete[] outputImage;

    return result;
}