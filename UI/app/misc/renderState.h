#pragma once

enum class renderState {
    Idle = 0,
    InProgress = 1,
    Cancelled = 2,
    Denoising = 3,
    PostProcessing = 4,
};
