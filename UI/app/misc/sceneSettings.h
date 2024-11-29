#pragma once

enum effects { none, bloom, glow, csb, steinberg };

class sceneSettings
{
public:
    int height = 0;
    int width = 0;
    int depth = 0;
    int spp = 0;
    std::string aspectRatio;
    std::string outputFilePath;

    int fx_index = 0;
    std::string fx_args;
};
