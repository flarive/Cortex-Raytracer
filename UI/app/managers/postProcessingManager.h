#pragma once

#include <windows.h>
#include "renderManager.h"

class postProcessingManager
{
public:

    postProcessingManager(renderManager& renderer, int renderWidth, int renderHeight);
    virtual ~postProcessingManager() = default;

    DWORD loadPostProcessedImage(const char* outputFilePath);
    HRESULT runPostProcessor(std::string externalProgram, std::string arguments, std::string outputPath);
    static DWORD __stdcall readPostProcessorOutputAsync(LPVOID argh);


private:
    renderManager& m_renderer;

    HANDLE m_readStandardOutputPostProcessorThread = NULL;


    char* m_savePostProcessedFilePath = NULL;

    HANDLE m_hChildStd_OUT_Rd2 = NULL;
    HANDLE m_hChildStd_OUT_Wr2 = NULL;


    int m_renderWidth = 0;
    int m_renderHeight = 0;

    PROCESS_INFORMATION m_pi;
};
