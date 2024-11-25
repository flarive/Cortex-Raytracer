#pragma once

#include <Windows.h>
#include "renderManager.h"

#include "../misc/renderState.h"
#include "../utilities/timer.h"

class denoiserManager
{
public:

    denoiserManager(const renderManager& renderer, const timer& renderTimer);
    DWORD loadDenoisedImage(const char* outputFilePath);
    HRESULT runDenoiser(std::string externalProgram, std::string arguments, std::string outputPath, int renderWidth, int renderHeight);
    DWORD __stdcall readDenoiserOutputAsync(void* argh);


private:
    renderManager m_renderer;

    HANDLE m_readStandardOutputDenoiserThread = NULL;

    std::string m_saveDenoisedFilePath;

    HANDLE m_hChildStd_OUT_Rd2 = NULL;
    HANDLE m_hChildStd_OUT_Wr2 = NULL;

    bool m_isCanceled = false;


    renderState m_renderStatus = renderState::Idle;
    float m_renderProgress = 0.0f;
    float m_averageRemaingTimeMs = 0;

    bool m_isRendering = false;

    timer m_renderTimer;

    int m_renderWidth = 0;
    int m_renderHeight = 0;
};
