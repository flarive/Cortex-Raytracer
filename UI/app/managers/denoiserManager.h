#pragma once

#include <windows.h>
#include "renderManager.h"

#include "../misc/renderState.h"
#include "../utilities/timer.h"

class denoiserManager
{
public:

    denoiserManager(renderManager& renderer, int renderWidth, int renderHeight);
    virtual ~denoiserManager() = default;

    DWORD loadDenoisedImage(const char* outputFilePath);
    HRESULT runDenoiser(std::string externalProgram, std::string arguments, std::string outputPath);
    static DWORD __stdcall readDenoiserOutputAsync(LPVOID argh);


private:
    renderManager& m_renderer;

    HANDLE m_readStandardOutputDenoiserThread = NULL;


    char* m_saveDenoisedFilePath = NULL;

    HANDLE m_hChildStd_OUT_Rd2 = NULL;
    HANDLE m_hChildStd_OUT_Wr2 = NULL;


    int m_renderWidth = 0;
    int m_renderHeight = 0;

    PROCESS_INFORMATION m_pi;
};
