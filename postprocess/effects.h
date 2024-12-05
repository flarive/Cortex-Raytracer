#pragma once

#include <glm/glm.hpp>

#include <string>

enum pp_effect { none, bloom, denoise, csb, floydsteinberg };


inline static std::string to_string(pp_effect fx)
{
    std::string result;

    switch (fx)
    {
    case pp_effect::none: result = "None";
        break;
    case pp_effect::bloom: result = "Bloom";
        break;
    case pp_effect::denoise: result = "Denoise";
        break;
    case pp_effect::csb: result = "Contrast Saturation Brightness";
        break;
    case pp_effect::floydsteinberg: result = "Floyd Steinberg dithering";
        break;
    }

    return result;
}

namespace pp_effect_bloom
{
    static float threshold = 0.0f;
    static float radius = 0.0f;
    static float intensity = 1.0f;
    static float max_bloom = 1.0f;
}

namespace pp_effect_denoise
{
}

namespace pp_effect_csb
{
    static float contrast = 1.0f;
    static float saturation = 1.0f;
    static float brightness = 1.0f;
}

namespace pp_effect_floydsteinberg
{
}