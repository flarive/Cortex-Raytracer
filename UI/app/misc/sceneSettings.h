#pragma once

#include <string>
#include <variant>
#include <map>

using pvar = std::variant<int, std::string, float>;
using pmap = std::map<std::string, pvar>;

class sceneSettings
{
public:
    int height = 0;
    int width = 0;
    int depth = 0;
    int spp = 0;
    std::string aspectRatio;
    std::string outputFilePath;

    short fx_index = 0;
    std::string fx_args;

    pmap fx_params;
};
