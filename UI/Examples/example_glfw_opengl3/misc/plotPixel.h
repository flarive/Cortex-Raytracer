#pragma once

struct plotPixel
{
public:
    unsigned int x = 0;
    unsigned int y = 0;

    unsigned int r = 0;
    unsigned int g = 0;
    unsigned int b = 0;
    unsigned int a = 0;


    plotPixel()
    {
    }


    plotPixel(unsigned int _x, unsigned int _y, unsigned int _r, unsigned int _g, unsigned int _b, unsigned int _a)
        : x(_x), y(_y), r(_r), g(_g), b(_b), a(_a)
    {
    }
};
