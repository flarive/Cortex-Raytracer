#pragma once

#include "plotPixel.h"

#include <string>
#include <map>
#include <vector>

class renderManager
{

public:

    renderManager();
    renderManager(unsigned int _width, unsigned int _height);

    void addPixelToFrameBuffer(int x, int y, int r, int g, int b, int a);
    unsigned char* getFrameBuffer();

    void addPixel(unsigned int lineCount, plotPixel* plotPixel);

    plotPixel* parsePixelEntry(unsigned int _index, std::string _entry);

    bool isFullyRendered();

    void render();


private:
    unsigned int width = 0;
    unsigned int height = 0;

    unsigned char* buffer = nullptr;

    std::map<unsigned int, plotPixel> pixels;
    std::vector<unsigned int> drawn;
};
