#pragma once

#include "../misc/pixel.h"
#include "../misc/renderState.h"

#include <string>
#include <map>
#include <memory>

class renderManager
{

public:

    // Default Constructor
    renderManager() = default;

    // Copy Constructor
    //renderManager(const renderManager& other);

    //// Copy Assignment Operator
    //renderManager& operator=(const renderManager& other);

    //// Move Constructor
    //renderManager(renderManager&& other) noexcept;

    //// Move Assignment Operator
    //renderManager& operator=(renderManager&& other) noexcept;

    // Destructor
    ~renderManager() = default;

    void initFromWidth(unsigned int _width, double _aspectRatio);
    void initFromHeight(unsigned int _height, double _aspectRatio);

    void addPixelToFrameBuffer(unsigned int x, unsigned int y, unsigned int r, unsigned int g, unsigned int b, unsigned int a);
    std::unique_ptr<unsigned char[]>& getFrameBuffer();
    unsigned long getFrameBufferSize();

    void addPixel(unsigned int index, pixel* plotPixel);

    pixel* parsePixelEntry(std::string _entry);

    bool isFullyRendered();

    void renderAll();
    void renderLine(unsigned int y);

    void clearFrameBuffer(bool clearBuffer = true);

    int getWidth();
    int getHeight();

    float getRenderProgress();
    unsigned int getRenderedLines();
    unsigned int getRemainingLines();


    renderState renderStatus = renderState::Idle;
    float renderProgress = 0.0f;

    bool isRenderable = false;
    bool isRendering = false;
    bool isCanceled = false;

private:
    unsigned int width = 0;
    unsigned int height = 0;
    double ratio = 0.0;

    std::unique_ptr<unsigned char[]> buffer = nullptr;

    std::map<unsigned int, pixel> pixels;
    std::map<unsigned int, bool> drawn;
};
