#pragma once

#include <windows.h>
#include "renderManager.h"

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

    HANDLE m_readStandardOutputDenoiserThread = INVALID_HANDLE_VALUE;


    char* m_saveDenoisedFilePath = NULL;

    HANDLE m_hChildStd_OUT_Rd2 = INVALID_HANDLE_VALUE;
    HANDLE m_hChildStd_OUT_Wr2 = INVALID_HANDLE_VALUE;


    int m_renderWidth = 0;
    int m_renderHeight = 0;

    PROCESS_INFORMATION m_pi;
};
