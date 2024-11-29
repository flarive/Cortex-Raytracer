#pragma once

#include <iostream>
#include <sstream>
#include <windows.h>
#include "stb_image.h"
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

class helpers
{
public:
    static double getRatio(std::string value)
    {
        if (value.empty())
        {
            return 0.0;
        }

        double p1 = 0, p2 = 0;

        std::stringstream test(value);
        std::string segment;

        unsigned int loop = 0;
        while (getline(test, segment, ':'))
        {
            if (loop == 0)
            {
                p1 = stoul(segment, 0, 10);
            }
            else if (loop == 1)
            {
                p2 = stoul(segment, 0, 10);
            }

            loop++;
        }

        if (p1 > 0 && p2 > 0)
        {
            return p1 / p2;
        }

        return 0.0;
    }

    
};


// Function to load icon from resources
static GLFWimage loadIconFromResource(int resourceId)
{
    HMODULE hModule = GetModuleHandle(NULL);
    HRSRC hResource = FindResource(hModule, MAKEINTRESOURCE(resourceId), RT_RCDATA);
    if (!hResource) {
        fprintf(stderr, "Failed to find resource\n");
        exit(EXIT_FAILURE);
    }

    HGLOBAL hResourceLoaded = LoadResource(hModule, hResource);
    if (!hResourceLoaded) {
        fprintf(stderr, "Failed to load resource\n");
        exit(EXIT_FAILURE);
    }

    LPVOID pResourceData = LockResource(hResourceLoaded);
    if (!pResourceData) {
        fprintf(stderr, "Failed to lock resource\n");
        exit(EXIT_FAILURE);
    }

    DWORD size = SizeofResource(hModule, hResource);
    if (size == 0) {
        fprintf(stderr, "Invalid resource size\n");
        exit(EXIT_FAILURE);
    }

    // Assuming the icon is a PNG file inside the resource
    GLFWimage image;
    int width, height, channels;
    unsigned char* data = stbi_load_from_memory((const stbi_uc*)pResourceData, size, &width, &height, &channels, 4);
    if (!data) {
        fprintf(stderr, "Failed to load image from resource\n");
        exit(EXIT_FAILURE);
    }

    image.width = width;
    image.height = height;
    image.pixels = data;

    return image;
}

