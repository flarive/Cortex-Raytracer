#include "renderManager.h"



#include <algorithm>
#include <string>
#include <vector>
#include <sstream>
#include <stdexcept>
#include <iostream>


using namespace std;


renderManager::renderManager()
{

}

renderManager::renderManager(unsigned int _width, unsigned int _height)
{
    width = _width;
    height = _height;

    // init render buffer
    buffer = new unsigned char[width * height * 4];
}

unsigned char* renderManager::getFrameBuffer()
{
    return buffer;
}

void renderManager::addPixel(unsigned int lineCount, plotPixel *plotPixel)
{
    pixels.emplace(lineCount, *plotPixel);
}


void renderManager::addPixelToFrameBuffer(int x, int y, int r, int g, int b, int a)
{
    auto fixedY = height - 1 - y;

    buffer[4 * (fixedY * width + x) + 0] = r;
    buffer[4 * (fixedY * width + x) + 1] = g;
    buffer[4 * (fixedY * width + x) + 2] = b;
    buffer[4 * (fixedY * width + x) + 3] = a;
}


bool renderManager::isFullyRendered()
{
    if (drawn.size() < ((width * height) - 1))
    {
        return false;
    }

    return true;
}

void renderManager::render()
{
    if (!isFullyRendered())
    {
        for (int y = 0; y < height; ++y)
        {
            for (int x = 0; x < width; ++x)
            {
                unsigned int index = (y * width) + x;
                //cout << "(" << y << "* " << SCREEN_WIDTH << ") + " << x << " >>> index " << index << "\n !!!!";

                if (std::find(drawn.begin(), drawn.end(), index) == drawn.end())
                {
                    /* v does not contain x */
                    if (pixels.find(index) != pixels.end())
                    {
                        // found
                        plotPixel pixel;
                        try
                        {
                            pixel = pixels.at(index);
                        }
                        catch (out_of_range& const e)
                        {
                            cerr << e.what() << std::endl;

                            // draw debug red pixel
                            pixel.x = x;
                            pixel.y = y;
                            pixel.r = 255;
                            pixel.g = 0;
                            pixel.b = 0;
                            pixel.a = 255;
                        }

                        addPixelToFrameBuffer(pixel.x, pixel.y, pixel.r, pixel.g, pixel.b, 255);
                        drawn.push_back(index);
                    }
                }
            }
        }
    }
    else
    {
        cout << "Finished !!!\n";
    }
}



plotPixel* renderManager::parsePixelEntry(unsigned int _index, string _entry)
{
    if (_entry.empty())
    {
        // return black pixel
        return nullptr;
    }

    auto count = ranges::count(_entry, ' ');
    if (count == 2)
    {
        plotPixel* pixel = new plotPixel();

        stringstream test(_entry);
        string segment;

        unsigned int loop = 0;
        while (getline(test, segment, ' '))
        {
            if (loop == 0)
            {
                // r
                if (!segment.empty())
                {
                    unsigned long lresult = stoul(segment, 0, 10);
                    pixel->r = lresult;
                }
            }
            else if (loop == 1)
            {
                // g
                if (!segment.empty())
                {
                    unsigned long lresult = stoul(segment, 0, 10);
                    pixel->g = lresult;
                }
            }
            else if (loop == 2)
            {
                // b
                if (!segment.empty())
                {
                    unsigned long lresult = stoul(segment, 0, 10);
                    pixel->b = lresult;
                }
            }

            loop++;
        }

        pixel->x = _index % width;
        pixel->y = _index / width;

        return pixel;
    }

    return nullptr;
}
