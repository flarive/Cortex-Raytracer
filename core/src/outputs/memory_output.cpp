#include "memory_output.h"

#include "../utilities/interval.h"

#include <string>
#include <format>
#include <iostream>

int memory_output::init_output(const size_t dataSize)
{
    const char* filePath = "mmap_example.bin";
    //const size_t dataSize = 1024; // Size of the data to be written


    wchar_t wfilePath[16];
    mbstowcs(wfilePath, filePath, strlen(filePath) + 1);//Plus null


    // Create a file handle
    m_hFile = CreateFile(
        wfilePath,
        GENERIC_READ | GENERIC_WRITE,
        0,
        nullptr,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        nullptr
    );

    if (m_hFile == INVALID_HANDLE_VALUE) {
        std::cerr << "Error opening file: " << GetLastError() << std::endl;
        return 1;
    }

    // Create a file mapping object
    m_hMapFile = CreateFileMapping(
        m_hFile,
        nullptr,
        PAGE_READWRITE,
        0,
        dataSize,
        nullptr
    );

    if (m_hMapFile == nullptr) {
        std::cerr << "Error creating file mapping: " << GetLastError() << std::endl;
        CloseHandle(m_hFile);
        return 1;
    }

    // Map a view of the file into the address space of the calling process
    m_lpBase = MapViewOfFile(
        m_hMapFile,
        FILE_MAP_WRITE,
        0,
        0,
        dataSize
    );

    if (m_lpBase == nullptr) {
        std::cerr << "Error mapping view of file: " << GetLastError() << std::endl;
        CloseHandle(m_hMapFile);
        CloseHandle(m_hFile);
        return 1;
    }
}


int memory_output::write_to_output(int x, int y, color pixel_color) const
{
    //// Write the translated [0,255] value of each color component.
    //// Static Variable gets constructed only once no matter how many times the function is called.
    static const interval intensity(0.000, 0.999);

    // not the fastest, can be improved !!!
    std::string aaa = std::format("p {} {} {} {} {}\n", x, y,
        static_cast<int>(256 * intensity.clamp(pixel_color.r())),
        static_cast<int>(256 * intensity.clamp(pixel_color.g())),
        static_cast<int>(256 * intensity.clamp(pixel_color.b())));


    // Write data to the memory-mapped file
    const char* message = aaa.c_str();
    memcpy(m_lpBase, message, strlen(message) + 1); // Include null terminator

    return 0;
}


int memory_output::clean_output()
{
    // Clean up
    UnmapViewOfFile(m_lpBase);
    CloseHandle(m_hMapFile);
    CloseHandle(m_hFile);

    //std::cout << "Writer: Data written to memory-mapped file." << std::endl;
    return 0;
}
