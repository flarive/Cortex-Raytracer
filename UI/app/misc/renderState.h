#pragma once

#include <string>

enum class renderState {
    Idle = 0,
    InProgress = 1,
    Cancelled = 2,
    Denoising = 3,
    PostProcessing = 4,
};

inline static std::string to_string(renderState state)
{
    std::string result;

    switch (state)
    {
    case renderState::Idle: result = "Idle";
        break;
    case renderState::InProgress: result = "In progress...";
        break;
    case renderState::Cancelled: result = "Cancelled";
        break;
    case renderState::Denoising: result = "Denoising";
        break;
    case renderState::PostProcessing: result = "PostProcessing";
        break;
    }

    return result;
}
