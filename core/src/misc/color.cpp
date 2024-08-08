#include "color.h"

#include <string>
#include <format>



color::color() : c{ 0,0,0,1 }
{
}

color::color(double c0) : c{ c0, c0, c0, 1 }
{
}

color::color(double c0, double c1, double c2) : c{ c0, c1, c2, 1 }
{
}

color::color(double c0, double c1, double c2, double c3) : c{ c0, c1, c2, c3 }
{
}

double color::r() const { return c[0]; }
double color::g() const { return c[1]; }
double color::b() const { return c[2]; }
double color::a() const { return c[3]; }

void color::r(double r) { c[0] = r; }
void color::g(double g) { c[1] = g; }
void color::b(double b) { c[2] = b; }
void color::a(double a) { c[3] = a; }

color color::operator-() const
{
    return color(-c[0], -c[1], -c[2]);
}

double color::operator[](int i) const
{
    return c[i];
}

double& color::operator[](int i)
{
    return c[i];
}

color& color::operator+=(const color& v)
{
    if (v.c[3] == 0)
    {
        return *this;
    }
        
    c[0] += v.c[0];
    c[1] += v.c[1];
    c[2] += v.c[2];
    c[3] += v.c[3];

    if (c[3] > 1)
    {
        c[3] = 1;
    }
    return *this;
}

color& color::operator+=(double t)
{
    c[0] += t;
    c[1] += t;
    c[2] += t;
    return *this;
}

color& color::operator*=(double t)
{
    c[0] *= t;
    c[1] *= t;
    c[2] *= t;
    return *this;
}

color& color::operator*=(const color& v)
{
    c[0] *= v[0];
    c[1] *= v[1];
    c[2] *= v[2];
    return *this;
}

color& color::operator/=(double t)
{
    return *this *= 1 / t;
}

double color::length() const
{
    return sqrt(length_squared());
}

double color::length_squared() const
{
    return c[0] * c[0] + c[1] * c[1] + c[2] * c[2];
}

color color::white()
{
    return color(1, 1, 1);
}

color color::black()
{
    return color(0, 0, 0);
}

color color::red()
{
    return color(1, 0, 0);
}

color color::green()
{
    return color(0, 1, 0);
}

color color::blue()
{
    return color(0, 0, 1);
}

color color::yellow()
{
    return color(0, 1, 1);
}

color color::undefined()
{
    return color(-1, -1, -1);
}

double color::linear_to_gamma(double linear_component)
{
    return sqrt(linear_component);
}

color color::blend_colors(const color& front, const color& back, double alpha)
{
    return alpha * front + (1.0 - alpha) * back;
}

// Add this function to handle blending based on alpha value
color color::blend_with_background(const color& background, const color& object_color, float alpha)
{
    return (1.0f - alpha) * background + alpha * object_color;
}


color color::RGBtoHSV(color rgb)
{
    double max_val = ffmax(ffmax(rgb.r(), rgb.g()), rgb.b());
    double min_val = ffmin(ffmin(rgb.r(), rgb.g()), rgb.b());
    double delta_val = max_val - min_val;
    color hsv;

    if (delta_val > 0) {
        if (max_val == rgb.r()) {
            hsv.r(60 * (fmod(((rgb.g() - rgb.b()) / delta_val), 6)));
        }
        else if (max_val == rgb.g()) {
            hsv.r(60 * (((rgb.b() - rgb.r()) / delta_val) + 2));
        }
        else if (max_val == rgb.b()) {
            hsv.r(60 * (((rgb.r() - rgb.g()) / delta_val) + 4));
        }
        if (max_val > 0) {
            hsv.g(delta_val / max_val);
        }
        else {
            hsv.g(0);
        }
        hsv.b(max_val);
    }
    else {
        hsv.r(0);
        hsv.g(0);
        hsv.b(max_val);
    }
    if (hsv.r() < 0) {
        hsv.r(360 + hsv.r());
    }
    return(hsv);
}


color color::HSVtoRGB(color hsv)
{
    double chroma = hsv.b() * hsv.g();
    double fHPrime = fmod(hsv.r() / 60.0, 6);
    double x_val = chroma * (1 - std::fabs(fmod(fHPrime, 2) - 1));
    double m_val = hsv.b() - chroma;

    if (0 <= fHPrime && fHPrime < 1) {
        color rgb(chroma, x_val, 0);
        rgb += m_val;
        return(rgb);
    }
    else if (1 <= fHPrime && fHPrime < 2) {
        color rgb(x_val, chroma, 0);
        rgb += m_val;
        return(rgb);
    }
    else if (2 <= fHPrime && fHPrime < 3) {
        color rgb(0, chroma, x_val);
        rgb += m_val;
        return(rgb);
    }
    else if (3 <= fHPrime && fHPrime < 4) {
        color rgb(0, x_val, chroma);
        rgb += m_val;
        return(rgb);
    }
    else if (4 <= fHPrime && fHPrime < 5) {
        color rgb(x_val, 0, chroma);
        rgb += m_val;
        return(rgb);
    }
    else if (5 <= fHPrime && fHPrime < 6) {
        color rgb(chroma, 0, x_val);
        rgb += m_val;
        return(rgb);
    }
    else {
        color rgb(0, 0, 0);
        rgb += m_val;
        return(rgb);
    }
}

bool color::isValidColor()
{
    if (c[0] >= 0 && c[1] >= 0 && c[2] >= 0 && c[3] >= 0)
    {
        return true;
    }

    return false;
}

/// <summary>
/// Write pixel color to the output stream
/// </summary>
/// <param name="out"></param>
/// <param name="pixel_color"></param>
void color::write_color_to_output(std::ostream& out, int x, int y, color pixel_color)
{
    // Write the translated [0,255] value of each color component.
    out << static_cast<int>(255.999 * pixel_color.r()) << ' '
        << static_cast<int>(255.999 * pixel_color.g()) << ' '
        << static_cast<int>(255.999 * pixel_color.b()) << '\n';
}






/// <summary>
/// Write pixel color to the output stream with pixel sampling and gamma correction
/// </summary>
/// <param name="out"></param>
/// <param name="pixel_color"></param>
/// <param name="samples_per_pixel"></param>
void color::write_color_to_output(std::ostream& out, int x, int y, color pixel_color, int samples_per_pixel, bool gamma_correction)
{
    double r = pixel_color.r();
    double g = pixel_color.g();
    double b = pixel_color.b();

    // Replace NaN components with zero.
    if (r != r) r = 0.0;
    if (g != g) g = 0.0;
    if (b != b) b = 0.0;

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
        r = linear_to_gamma(r);
        g = linear_to_gamma(g);
        b = linear_to_gamma(b);

        //r = 0.5 * r;
        //g = 0.5 * g;
        //b = 0.5 * b;
    }

    // Write the translated [0,255] value of each color component.
    // Static Variable gets constructed only once no matter how many times the function is called.
    static const interval intensity(0.000, 0.999);

	out << "p " << x << " " << y << " "
		<< static_cast<int>(256 * intensity.clamp(r)) << " "
		<< static_cast<int>(256 * intensity.clamp(g)) << " "
		<< static_cast<int>(256 * intensity.clamp(b)) << "\n";
}


int color::init_memory_file(const size_t dataSize)
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


int color::write_color_to_memory(int x, int y, color pixel_color, int samples_per_pixel, bool gamma_correction)
{
    double r = pixel_color.r();
    double g = pixel_color.g();
    double b = pixel_color.b();

    // Replace NaN components with zero.
    if (r != r) r = 0.0;
    if (g != g) g = 0.0;
    if (b != b) b = 0.0;

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
        r = linear_to_gamma(r);
        g = linear_to_gamma(g);
        b = linear_to_gamma(b);

        //r = 0.5 * r;
        //g = 0.5 * g;
        //b = 0.5 * b;
    }

    // Write the translated [0,255] value of each color component.
    // Static Variable gets constructed only once no matter how many times the function is called.
    static const interval intensity(0.000, 0.999);

    // not the fastest, can be improved !!!
    std::string aaa = std::format("p {} {} {} {} {}\n", x, y, 
        static_cast<int>(256 * intensity.clamp(r)), 
        static_cast<int>(256 * intensity.clamp(g)), 
        static_cast<int>(256 * intensity.clamp(b)));
    

    // Write data to the memory-mapped file
    const char* message = aaa.c_str();
    memcpy(m_lpBase, message, strlen(message) + 1); // Include null terminator

    return 0;
}


int color::clean_memory_file()
{
    // Clean up
    UnmapViewOfFile(m_lpBase);
    CloseHandle(m_hMapFile);
    CloseHandle(m_hFile);

    //std::cout << "Writer: Data written to memory-mapped file." << std::endl;
    return 0;
}



// reader process

//#include <windows.h>
//#include <iostream>
//
//int main() {
//    const char* filePath = "mmap_example.bin";
//    const size_t dataSize = 1024; // Size of the data to be read
//
//    // Open the file
//    HANDLE hFile = CreateFile(
//        filePath,
//        GENERIC_READ,
//        0,
//        nullptr,
//        OPEN_EXISTING,
//        FILE_ATTRIBUTE_NORMAL,
//        nullptr
//    );
//
//    if (hFile == INVALID_HANDLE_VALUE) {
//        std::cerr << "Error opening file: " << GetLastError() << std::endl;
//        return 1;
//    }
//
//    // Open a file mapping object
//    HANDLE hMapFile = CreateFileMapping(
//        hFile,
//        nullptr,
//        PAGE_READONLY,
//        0,
//        dataSize,
//        nullptr
//    );
//
//    if (hMapFile == nullptr) {
//        std::cerr << "Error creating file mapping: " << GetLastError() << std::endl;
//        CloseHandle(hFile);
//        return 1;
//    }
//
//    // Map a view of the file into the address space of the calling process
//    LPVOID lpBase = MapViewOfFile(
//        hMapFile,
//        FILE_MAP_READ,
//        0,
//        0,
//        dataSize
//    );
//
//    if (lpBase == nullptr) {
//        std::cerr << "Error mapping view of file: " << GetLastError() << std::endl;
//        CloseHandle(hMapFile);
//        CloseHandle(hFile);
//        return 1;
//    }
//
//    // Read data from the memory-mapped file
//    char buffer[dataSize];
//    memcpy(buffer, lpBase, dataSize);
//
//    // Clean up
//    UnmapViewOfFile(lpBase);
//    CloseHandle(hMapFile);
//    CloseHandle(hFile);
//
//    std::cout << "Reader: Data read from memory-mapped file: " << buffer << std::endl;
//    return 0;
//}


