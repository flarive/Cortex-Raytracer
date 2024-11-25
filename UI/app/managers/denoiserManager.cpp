#include "denoiserManager.h"


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


#include <iostream>
#include <string>
#include <filesystem>
#include <thread>

denoiserManager::denoiserManager(const renderManager& renderer, const timer& renderTimer) : m_renderer(renderer), m_renderTimer(renderTimer)
{
}



HRESULT denoiserManager::runDenoiser(std::string externalProgram, std::string arguments, std::string outputPath, int renderWidth, int renderHeight)
{
    m_renderWidth = renderWidth;
    m_renderHeight = renderHeight;

    std::filesystem::path dir(std::filesystem::current_path());
    std::filesystem::path file(externalProgram);
    std::filesystem::path fullexternalProgramPath = dir / file;


    if (!exists(fullexternalProgramPath))
    {
        //_textBuffer.appendf("[ERROR] Denoiser exe not found !\n");
        //_scrollToBottom = true;
        return S_FALSE;
    }

    STARTUPINFO si;
    SECURITY_ATTRIBUTES saAttr;

    ZeroMemory(&saAttr, sizeof(saAttr));
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;

    // Create a pipe for the child process's STDOUT. 
    if (!CreatePipe(&m_hChildStd_OUT_Rd2, &m_hChildStd_OUT_Wr2, &saAttr, 0))
    {
        // log error
        return HRESULT_FROM_WIN32(GetLastError());
    }

    // Ensure the read handle to the pipe for STDOUT is not inherited.
    if (!SetHandleInformation(m_hChildStd_OUT_Rd2, HANDLE_FLAG_INHERIT, 0))
    {
        // log error
        return HRESULT_FROM_WIN32(GetLastError());
    }

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.hStdError = m_hChildStd_OUT_Wr2;
    si.hStdOutput = m_hChildStd_OUT_Wr2;
    si.dwFlags |= STARTF_USESTDHANDLES;

    ZeroMemory(&pi, sizeof(pi));

    std::string commandLine = fullexternalProgramPath.generic_string() + " " + arguments;

    // Start the child process. 
    if (!CreateProcessA(externalProgram.c_str(),           // No module name (use command line)
        (TCHAR*)commandLine.c_str(),    // Command line
        NULL,                           // Process handle not inheritable
        NULL,                           // Thread handle not inheritable
        TRUE,                           // Set handle inheritance
        CREATE_NO_WINDOW | CREATE_BREAKAWAY_FROM_JOB,               // No creation flags
        NULL,                           // Use parent's environment block
        NULL,                           // Use parent's starting directory 
        &si,                            // Pointer to STARTUPINFO structure
        &pi))                            // Pointer to PROCESS_INFORMATION structure
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }
    else
    {
        m_saveDenoisedFilePath = outputPath;

        m_readStandardOutputDenoiserThread = CreateThread(0, 0, readDenoiserOutputAsync, NULL, 0, NULL);
    }

    return S_OK;
}


DWORD __stdcall denoiserManager::readDenoiserOutputAsync(void* argh)
{
    UNREFERENCED_PARAMETER(argh);

    DWORD dwRead = 0;
    CHAR chBuf[1];
    bool bSuccess = false;

    std::string data;

    unsigned int indexPixel = 0;

    for (;;)
    {
        if (m_isCanceled)
        {
            return S_FALSE;
        }

        bSuccess = ReadFile(m_hChildStd_OUT_Rd2, chBuf, 1, &dwRead, NULL);
        if (!bSuccess || dwRead == 0)
        {
            continue;
        }

        data.append(&chBuf[0], dwRead);

        if (data.ends_with("\r\n"))
        {
            if (data == "[INFO] Denoised image saved successfully.\r\n")
            {
                if (!m_saveDenoisedFilePath.empty())
                {
                    // Allocate memory for outputPath to ensure it's still valid during thread execution
                    char* outputFilePathCopy = new char[m_saveDenoisedFilePath.size() + 1]; // +1 for null terminator
                    strcpy_s(outputFilePathCopy, m_saveDenoisedFilePath.size() + 1, m_saveDenoisedFilePath.c_str());

                    loadDenoisedImage(outputFilePathCopy);
                }
            }

            //_textBuffer.appendf(data.c_str());
            //_scrollToBottom = true;

            data.clear();

            if ((int)indexPixel > (m_renderWidth * m_renderHeight) - 2)
            {
                // Stop measuring time
                m_renderTimer.stop();

                m_renderer.clearFrameBuffer(false);

                m_renderStatus = renderState::Idle;
                m_renderProgress = 0.0f;
                m_averageRemaingTimeMs = 0;

                m_isRendering = false;
            }

            if (!bSuccess)
            {
                break;
            }
        }
    }

    return S_OK;
}

DWORD denoiserManager::loadDenoisedImage(const char* outputFilePath)
{
    if (outputFilePath == nullptr || outputFilePath[0] == '\0')
    {
        return 1;
    }

    int depth = 3; // Force 3 channels (RGB)

    int width, height, channels;
    // Load the image as floating-point data (STBI loads as float in the range [0, 1])
    unsigned char* imageData = stbi_load(outputFilePath, &width, &height, &channels, depth);
    if (imageData == nullptr)
    {
        std::cerr << "Failed to load image!" << std::endl;
        return 1;
    }

    double ratio = (double)width / (double)height;

    if (width > height)
    {
        m_renderer.initFromWidth(width, ratio);
    }
    else
    {
        m_renderer.initFromHeight(height, ratio);
    }

    int indexPixel = 0;

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            // Calculate the index of the pixel in the buffer
            int index = (y * width + x) * 3;

            // Extract the RGB values from the buffer
            unsigned char r = imageData[index];     // Red channel
            unsigned char g = imageData[index + 1]; // Green channel
            unsigned char b = imageData[index + 2]; // Blue channel

            m_renderer.addPixel(indexPixel, new pixel(x, y, static_cast<int>(r), static_cast<int>(g), static_cast<int>(b), 255));
            m_renderer.addPixelToFrameBuffer(x, y, r, g, b, 255);

            indexPixel++;
        }
    }

    m_renderer.renderAll();

    //isRendering = false;
    //renderStatus = renderState::Idle;
    //renderProgress = 100.0f;

    // Clean up resources
    stbi_image_free(imageData);  // Free the image data loaded by stbi

    // Don't forget to delete the dynamically allocated memory for outputFilePath
    delete[] outputFilePath;

    return 0;
}
