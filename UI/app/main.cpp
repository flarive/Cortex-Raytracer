// Dear ImGui: standalone example application for GLFW + OpenGL 3, using programmable pipeline
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)

// Learn about Dear ImGui:
// - FAQ                  https://dearimgui.com/faq
// - Getting Started      https://dearimgui.com/getting-started
// - Documentation        https://dearimgui.com/docs (same as your local docs/ folder).
// - Introduction, links and more at the top of imgui.cpp

//vcpkg install --triplet x64-windows-static
//$(SolutionDir)libs\libconfig\build\release\static\libconfig++s.lib
//$(SolutionDir)libs\glfw\lib-vc2022-64\glfw3_mt.lib

#include "../imgui.h"
#include "../imgui_internal.h"
#include "../backends/imgui_impl_glfw.h"
#include "../backends/imgui_impl_opengl3.h"
#include "themes/imgui_spectrum.h"
#include "managers/renderManager.h"
#include "managers/sceneManager.h"
#include "misc/scene.h"
#include "utilities/timer.h"
#include "utilities/helpers.h"
#include "misc/sceneSettings.h"
#include "resource.h"

#include "widgets/toggle/imgui_toggle.h"
#include "widgets/toggle/imgui_toggle_presets.h"
#include "widgets/toggle/imgui_toggle_palette.h"


#include <shlobj.h>
#include <shlwapi.h>
#include <objbase.h>
#include <chrono>
#include <thread>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <windows.h>
#include <iostream>

#include <stdio.h>
#define GL_SILENCE_DEPRECATION
//#if defined(IMGUI_IMPL_OPENGL_ES2)
//#include <GLES2/gl2.h>
//#endif
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

#include <string>
#include <filesystem>







// for simplicity 
using namespace std;
using namespace std::filesystem;
using namespace std::chrono;


// Function to load icon from resources
GLFWimage loadIconFromResource(int resourceId)
{
    HMODULE hModule = GetModuleHandle(NULL);
    HRSRC hResource = FindResource(hModule, MAKEINTRESOURCE(resourceId), RT_RCDATA);
    if (!hResource) {
        fprintf(stderr, "Failed to find resource\n");
        exit(EXIT_FAILURE);
    }

    HGLOBAL hResourceLoaded = LoadResource(hModule, hResource);
    if (!hResourceLoaded) {
        fprintf(stderr, "Failed to load resource\n");
        exit(EXIT_FAILURE);
    }

    LPVOID pResourceData = LockResource(hResourceLoaded);
    if (!pResourceData) {
        fprintf(stderr, "Failed to lock resource\n");
        exit(EXIT_FAILURE);
    }

    DWORD size = SizeofResource(hModule, hResource);
    if (size == 0) {
        fprintf(stderr, "Invalid resource size\n");
        exit(EXIT_FAILURE);
    }

    // Assuming the icon is a PNG file inside the resource
    GLFWimage image;
    int width, height, channels;
    unsigned char* data = stbi_load_from_memory((const stbi_uc*)pResourceData, size, &width, &height, &channels, 4);
    if (!data) {
        fprintf(stderr, "Failed to load image from resource\n");
        exit(EXIT_FAILURE);
    }

    image.width = width;
    image.height = height;
    image.pixels = data;

    return image;
}



static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}


int renderWidth = 512;
int renderHeight = 288;

static const char* renderRatios[] = { "16:9", "4:3", "3:2", "1:1", "2:3", "3:4", "9:16" };
std::string renderRatio = renderRatios[0];
static int ratio_current_idx = 0;

std::vector<std::string> deviceModes{};
static std::string deviceMode;
static int device_current_idx = 0;

std::vector<std::string> antialiasingModes{ "None", "Random (fast)", "MultiSamping (slower)"};
static std::string antialiasingMode;
static int antialiasing_current_idx = 0;

std::vector<std::string> randomizerModes{ "Default" };// , "std::mt19937", "std::minstd_rand", "std::minstd_rand0"};
static std::string randomizerMode;
static int randomizer_current_idx = 0;


bool is_ratio_landscape = true;
int renderSamplePerPixel = 100;
int renderMaxDepth = 100;

bool renderUseGammaCorrection = true;
bool renderAutoDenoise = true;

std::string saveFilePath;
std::string saveDenoisedFilePath;

static bool _scrollToBottom = false;
static ImGuiTextBuffer _textBuffer;


std::vector<scene> items_scenes{};

std::string sceneName;
static int scene_current_idx = 0;
static std::string latestSceneSelected;


const char* renderStatus = "Idle";
float renderProgress = 0.0;

bool isRenderable = false;
bool isRendering = false;
bool isCanceled = false;


#define BUFSIZE_NAMED_PIPES 24
#define BUFSIZE_STANDARD_OUTPUT 27
HANDLE m_hChildStd_OUT_Rd = NULL;
HANDLE m_hChildStd_OUT_Wr = NULL;

HANDLE m_hChildStd_OUT_Rd2 = NULL;
HANDLE m_hChildStd_OUT_Wr2 = NULL;

HANDLE m_readStandardOutputRaytracerThread = NULL;
HANDLE m_readStandardOutputDenoiserThread = NULL;
HANDLE m_readNamedPipesThread = NULL;
HANDLE m_renderRaytracerThread = NULL;
HANDLE m_renderDenoiserThread = NULL;

renderManager renderer;
sceneManager manager;

timer renderTimer;
double averageRemaingTimeMs = 0;

PROCESS_INFORMATION pi;
HANDLE ghJob = NULL;

HRESULT runRaytracer(string externalProgram, string arguments);
HRESULT runDenoiser(string externalProgram, string arguments, string outputPath);

DWORD loadDenoisedImage(const char* outputFilePath);


void cancelRendering()
{
    //signal all threads to exit
    isCanceled = true;

    // actually wait for the read output thread to exit
    WaitForSingleObject(m_readStandardOutputRaytracerThread, INFINITE);

    // cleanup the thread
    CloseHandle(m_readStandardOutputRaytracerThread);
    m_readStandardOutputRaytracerThread = NULL;


    _textBuffer.appendf("[INFO] Rendering cancelled !");
    _scrollToBottom = true;


    // actually wait for the render thread to exit
    WaitForSingleObject(m_renderRaytracerThread, INFINITE);

    // cleanup the thread
    CloseHandle(m_renderRaytracerThread);
    m_renderRaytracerThread = NULL;

    // kill exe process
    if (pi.hProcess)
        TerminateProcess(pi.hProcess, 0);
}


DWORD __stdcall renderLineAsync(unsigned int* lineIndex)
{
    if (isCanceled)
    {
        return S_FALSE;
    }

    renderer.renderLine(*lineIndex);

    return S_OK;
}

DWORD __stdcall readNamedPipeAsync(void* argh)
{
    UNREFERENCED_PARAMETER(argh);

    DWORD dwRead = 0;
    CHAR chBuf[BUFSIZE_NAMED_PIPES];
    bool bSuccess = false;

    string data = string();

    unsigned int indexPixel = 0;
    unsigned int indexLine = 0;

    int pack = 0;

    // Start measuring time
    renderTimer.start();



    // Connect to the named pipe
    const char* pipeName = "\\\\.\\pipe\\MyNamedPipe";

    _textBuffer.appendf("[INFO] Trying to connect to the pipe\r\n");
    _scrollToBottom = true;


    HANDLE hPipe = CreateFile(
        pipeName,             // Pipe name
        GENERIC_READ,         // Read access
        0,                    // No sharing
        nullptr,              // Default security attributes
        OPEN_EXISTING,        // Opens existing pipe
        0,                    // Default attributes
        nullptr               // No template file
    );

    if (hPipe == INVALID_HANDLE_VALUE) {
        _textBuffer.appendf("[ERROR] Failed to connect to pipe. Error: " + GetLastError());
        _scrollToBottom = true;

        return S_FALSE;
    }

    for (;;)
    {
        if (isCanceled)
        {
            CloseHandle(hPipe);
            return S_FALSE;
        }

        // Read from the named pipe instead of standard output
        bSuccess = ReadFile(hPipe, chBuf, BUFSIZE_NAMED_PIPES, &dwRead, nullptr);
        if (!bSuccess || dwRead == 0)
        {
            // nothing more to read
            continue;
        }

        data = std::string(chBuf, dwRead);


        pixel* plotPixel = renderer.parsePixelEntry(data);
        if (plotPixel)
        {
            renderer.addPixel(indexPixel, plotPixel);
            indexPixel++;
        }

        // Wait for a full line to be calculated before displaying it to screen
        if (pack >= renderer.getWidth())
        {
            // call render thread
            m_renderRaytracerThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)renderLineAsync, &indexLine, 0, nullptr);
            WaitForSingleObject(m_renderRaytracerThread, INFINITE);

            pack = -1;
            indexLine++;
        }

        pack++;


        if (isRendering && (int)indexPixel > (renderWidth * renderHeight))
        {
            // Stop measuring time
            renderTimer.stop();

            renderer.clearFrameBuffer(false);

            renderStatus = "Idle";
            renderProgress = 0.0;
            averageRemaingTimeMs = 0;
            isRendering = false;
        }

        if (!bSuccess)
        {
            break;
        }
    }

    return S_OK;
}


DWORD __stdcall readOuputAsync(void* argh)
{
    UNREFERENCED_PARAMETER(argh);

    DWORD dwRead = 0;
    CHAR chBuf[1];
    bool bSuccess = false;

    string data = string();

    for (;;)
    {
        if (isCanceled)
        {
            return S_FALSE;
        }

        bSuccess = ReadFile(m_hChildStd_OUT_Rd, chBuf, 1, &dwRead, NULL);
        if (!bSuccess || dwRead == 0)
        {
            continue;
        }

        data.append(&chBuf[0], dwRead);

        if (data.ends_with("\r\n"))
        {
            if (data.starts_with("["))
            {
                if (data == "[INFO] Waiting for client to connect...\r\n")
                {
                    // start listening from named pipe
                    m_readNamedPipesThread = CreateThread(0, 0, readNamedPipeAsync, NULL, 0, NULL);
                }
                else if (data.starts_with("[INFO] Image saved to"))
                {
                    // apply denoiser
                    if (renderAutoDenoise)
                    {
                        // apply denoise
                        renderStatus = "Denoising";

                        _textBuffer.appendf("[INFO] Calling denoiser\n");
                        _scrollToBottom = true;

                        std::string outputPath = std::string(saveFilePath).replace(saveFilePath.size() - 4, 1, "_denoised.");

                        runDenoiser("CortexRTDenoiser.exe", std::format("-quiet -input {} -output {} -hdr {}", saveFilePath, outputPath, 0), outputPath);
                    }
                }
                
                _textBuffer.appendf(data.c_str());
                _scrollToBottom = true;
            }

            data.clear();
        }

        if (!bSuccess)
        {
            break;
        }
    }

    return S_OK;
}


DWORD __stdcall readDenoiserOutputAsync(void* argh)
{
    UNREFERENCED_PARAMETER(argh);

    DWORD dwRead = 0;
    CHAR chBuf[1];
    bool bSuccess = false;

    string data = string();


    unsigned int indexPixel = 0;

    for (;;)
    {
        if (isCanceled)
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
                if (!saveDenoisedFilePath.empty())
                {
                    // Allocate memory for outputPath to ensure it's still valid during thread execution
                    char* outputFilePathCopy = new char[saveDenoisedFilePath.size() + 1]; // +1 for null terminator
                    strcpy_s(outputFilePathCopy, saveDenoisedFilePath.size() + 1, saveDenoisedFilePath.c_str());

                    loadDenoisedImage(outputFilePathCopy);
                }
            }

            _textBuffer.appendf(data.c_str());
            _scrollToBottom = true;
            

            data.clear();

            if ((int)indexPixel > (renderWidth * renderHeight) - 2)
            {
                // Stop measuring time
                renderTimer.stop();

                renderer.clearFrameBuffer(false);

                renderStatus = "Idle";
                renderProgress = 0.0;
                averageRemaingTimeMs = 0;

                isRendering = false;
            }

            if (!bSuccess)
            {
                break;
            }
        }
    }

    return S_OK;
}



DWORD loadDenoisedImage(const char* outputFilePath)
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
        renderer.initFromWidth(width, ratio);
    }
    else
    {
        renderer.initFromHeight(height, ratio);
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

            renderer.addPixel(indexPixel, new pixel(x, y, static_cast<int>(r), static_cast<int>(g), static_cast<int>(b), 255));
            renderer.addPixelToFrameBuffer(x, y, r, g, b, 255);

            indexPixel++;
        }
    }

    renderer.renderAll();

    isRendering = false;
    renderStatus = "Idle";
    renderProgress = 100.0;

    // Clean up resources
    stbi_image_free(imageData);  // Free the image data loaded by stbi

    // Don't forget to delete the dynamically allocated memory for outputFilePath
    delete[] outputFilePath;

    return 0;
}



/// <summary>
/// https://stackoverflow.com/questions/42402673/createprocess-and-capture-stdout
/// </summary>
/// <param name="externalProgram"></param>
/// <param name="arguments"></param>
/// <returns></returns>
HRESULT runRaytracer(string externalProgram, string arguments)
{
    path dir(current_path());
    path file(externalProgram);
    path fullexternalProgramPath = dir / file;


    if (!exists(fullexternalProgramPath))
    {
        _textBuffer.appendf("[ERROR] Renderer exe not found !\n");
        _scrollToBottom = true;

        return S_FALSE;
    }

    if (ghJob == NULL)
    {
        ghJob = CreateJobObject(NULL, NULL); // GLOBAL
        if (ghJob == NULL)
        {
            _textBuffer.appendf("[ERROR] Could not create job object\n");
            _scrollToBottom = true;

            return HRESULT_FROM_WIN32(GetLastError());
        }
        else
        {
            JOBOBJECT_EXTENDED_LIMIT_INFORMATION jeli = { 0 };
            jeli.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
            if (0 == SetInformationJobObject(ghJob, JobObjectExtendedLimitInformation, &jeli, sizeof(jeli)))
            {
                _textBuffer.appendf("[ERROR] Could not SetInformationJobObject\n");
                _scrollToBottom = true;
            }
        }
    }


    STARTUPINFO si;
    SECURITY_ATTRIBUTES saAttr;

    ZeroMemory(&saAttr, sizeof(saAttr));
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;

    // Create a pipe for the child process's STDOUT. 
    if (!CreatePipe(&m_hChildStd_OUT_Rd, &m_hChildStd_OUT_Wr, &saAttr, 0))
    {
        // log error
        return HRESULT_FROM_WIN32(GetLastError());
    }

    // Ensure the read handle to the pipe for STDOUT is not inherited.
    if (!SetHandleInformation(m_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0))
    {
        // log error
        return HRESULT_FROM_WIN32(GetLastError());
    }

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.hStdError = m_hChildStd_OUT_Wr;
    si.hStdOutput = m_hChildStd_OUT_Wr;
    si.dwFlags |= STARTF_USESTDHANDLES;

    ZeroMemory(&pi, sizeof(pi));

    string commandLine = fullexternalProgramPath.generic_string() + " " + arguments;

    // Start the the raytracer in a child process. 
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
        // process fail
        return HRESULT_FROM_WIN32(GetLastError());
    }
    else
    {
        if (ghJob != NULL)
        {
            if (0 == AssignProcessToJobObject(ghJob, pi.hProcess))
            {
                _textBuffer.appendf("[ERROR] Could not AssignProcessToJobObject\n");
                _scrollToBottom = true;
                // Optionally, return or handle the error here
            }
        }

        // start a new thread to read the raytracer output
        m_readStandardOutputRaytracerThread = CreateThread(0, 0, readOuputAsync, NULL, 0, NULL);
    }

    return S_OK;
}



HRESULT runDenoiser(string externalProgram, string arguments, string outputPath)
{
    path dir(current_path());
    path file(externalProgram);
    path fullexternalProgramPath = dir / file;


    if (!exists(fullexternalProgramPath))
    {
        _textBuffer.appendf("[ERROR] Denoiser exe not found !\n");
        _scrollToBottom = true;
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

    string commandLine = fullexternalProgramPath.generic_string() + " " + arguments;

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
        saveDenoisedFilePath = outputPath;

        m_readStandardOutputDenoiserThread = CreateThread(0, 0, readDenoiserOutputAsync, NULL, 0, NULL);
    }

    return S_OK;
}


void selectScene(int n, GLFWwindow* window)
{
    sceneName = items_scenes.at(n).getPath();

    const path path = sceneName;
    glfwSetWindowTitle(window, path.filename().string().c_str());

    latestSceneSelected = path.string();

    std::unique_ptr<sceneSettings> settings = manager.readSceneSettings(path.string());
    if (settings)
    {
        renderWidth = settings->width;
        renderHeight = settings->height;
        renderRatio = settings->aspectRatio;
        renderMaxDepth = settings->depth;
        renderSamplePerPixel = settings->spp;
        saveFilePath = settings->outputFilePath;

        if (saveFilePath.empty())
        {
            // Buffer to hold the temporary file name
            char tempName[L_tmpnam];

            // Generate the temporary file name
            if (std::tmpnam(tempName)) {
                // Add ".png" extension to the generated file name
                saveFilePath = std::string(tempName) + ".png";
            }
            else {
                std::cerr << "Error generating temporary file name!" << std::endl;
            }
        }

        const char* target = renderRatio.c_str();
        int arraySize = sizeof(renderRatios) / sizeof(renderRatios[0]);

        ratio_current_idx = -1;
        for (int i = 0; i < arraySize; ++i) {
            if (strcmp(renderRatios[i], target) == 0) {
                ratio_current_idx = i;
                break;
            }
        }

        double ratio = helpers::getRatio(renderRatio);

        if (renderWidth > renderHeight)
        {
            renderer.initFromWidth(renderWidth, ratio);
            renderHeight = renderer.getHeight();
        }
        else
        {
            renderer.initFromHeight(renderHeight, ratio);
            renderWidth = renderer.getWidth();
        }

        glfwSetWindowSize(window, renderWidth, renderHeight);
    }

    isRenderable = scene_current_idx > 0;
}


static void* UserData_ReadOpen(ImGuiContext*, ImGuiSettingsHandler*, const char* name)
{
    UNREFERENCED_PARAMETER(name);

    return (void*)"LastSelectedScene";
}

static void UserData_ReadLine(ImGuiContext*, ImGuiSettingsHandler*, void* entry, const char* line)
{
    if (entry && std::strcmp(static_cast<const char*>(entry), "LastSelectedScene") == 0)
    {
        std::string str_line(line);
        std::string str_key("LastSelectedScene");
        if (str_line.length() > str_key.length())
        {
            latestSceneSelected = str_line.substr(str_key.length() + 1, str_line.length() - str_key.length());
        }
    }
}

static void UserData_WriteAll(ImGuiContext* ctx, ImGuiSettingsHandler* handler, ImGuiTextBuffer* buf)
{
    UNREFERENCED_PARAMETER(ctx);
    UNREFERENCED_PARAMETER(handler);

    buf->appendf("[%s][%s]\n", "UserData", "Path");
    buf->appendf("LastSelectedScene=%s\n", latestSceneSelected.c_str());
    buf->append("\n");
}


// Main code
int main(int, char**)
{
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return S_FALSE;

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    const char* glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(512, 288, "RayTracer", nullptr, nullptr);
    if (window == nullptr)
        return S_FALSE;

    glfwMakeContextCurrent(window);



    // Load the icon from resources
    GLFWimage icon = loadIconFromResource(PNG_ICON);

    // Set the window icon
    glfwSetWindowIcon(window, 1, &icon);

    // Free the loaded image data
    stbi_image_free(icon.pixels);




    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();


    // Write custom data in imgui.ini
    ImGuiSettingsHandler ini_handler;
    ini_handler.TypeName = "UserData";
    ini_handler.TypeHash = ImHashStr("UserData");
    ini_handler.ReadOpenFn = UserData_ReadOpen;
    ini_handler.ReadLineFn = UserData_ReadLine;
    ini_handler.WriteAllFn = UserData_WriteAll;
    ImGui::AddSettingsHandler(&ini_handler);

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    //io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
    //io.ConfigViewportsNoAutoMerge = true;
    io.ConfigViewportsNoTaskBarIcon = true;
    //io.ConfigViewportsNoDecoration = false;
    //io.ConfigViewportsNoDefaultParent = false;





    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.ChildRounding = 5.0f;
        style.TabRounding = 5.f;
        style.FrameRounding = 5.f;
        style.GrabRounding = 5.f;
        style.PopupRounding = 5.f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;

        style.ItemSpacing.y = 8.0; // vertical padding between widgets
        style.FramePadding.x = 8.0; // better widget horizontal padding
        style.FramePadding.y = 4.0; // better widget vertical padding
    }



    // Apply Adobe Spectrum theme
    //https://github.com/adobe/imgui/blob/master/docs/Spectrum.md#imgui-spectrum
    ImGui::Spectrum::StyleColorsSpectrum();
    ImGui::Spectrum::LoadFont(17.0);


    ImGui::LoadIniSettingsFromDisk(ImGui::GetIO().IniFilename);


    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);

    ImGui_ImplOpenGL3_Init(glsl_version);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    // - Our Emscripten build process allows embedding fonts to be accessible at runtime from the "fonts/" folder. See Makefile.emscripten for details.
    //io.Fonts->AddFontDefault();

    //https://github.com/ocornut/imgui/blob/master/docs/FONTS.md
    //io.Fonts->AddFontFromFileTTF("AdobeCleanRegular.otf", 18.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != nullptr);

    // Our state
    //bool show_demo_window = true;
    bool show_rendering_parameters = true;
    bool show_scenes_manager = true;
    ImVec4 clear_color = ImVec4(0.80f, 0.80f, 0.80f, 1.00f);


    manager.setScenesPath("../../data/scenes");
    items_scenes = manager.listAllScenes();

    items_scenes.insert(items_scenes.begin(), scene("Choose a scene", ""));

    if (!latestSceneSelected.empty())
    {
        int loop = 0;
        for (auto& element : items_scenes)
        {
            if (element.getPath() == latestSceneSelected)
            {
                scene_current_idx = loop;
                selectScene(scene_current_idx, window);
                break;
            }

            loop++;
        }
    }

    const unsigned int nbr_threads = std::thread::hardware_concurrency();

    for (unsigned int n = 1; n <= nbr_threads; n++)
    {
        if (n == 1)
            deviceModes.emplace_back("Mono thread");
        else
            deviceModes.emplace_back(std::format("Multi thread {} core", n));
    }

    device_current_idx = static_cast<int>(deviceModes.size()) - 1;
    deviceMode = deviceModes[device_current_idx];
    




    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();



        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();



   
        //if (show_demo_window)
        //    ImGui::ShowDemoWindow(&show_demo_window);
        
        ImGui::SetNextWindowSize(ImVec2(250, 600), ImGuiCond_FirstUseEver);

        if (show_rendering_parameters)
        {
            // Create rendering parameters window
            bool renderingParamsOpened = true;
            ImGui::Begin("Rendering parameters", &renderingParamsOpened, ImGuiWindowFlags_NoResize);


            ImGui::PushItemWidth(-1);

            scene scene_preview_value = items_scenes.at(scene_current_idx);
            if (ImGui::BeginCombo("Scenes", scene_preview_value.getName().c_str(), 0))
            {
                for (int n = 0; n < items_scenes.size(); n++)
                {
                    const bool is_selected = (scene_current_idx == n);
                    if (ImGui::Selectable(items_scenes.at(n).getName().c_str(), is_selected))
                    {
                        scene_current_idx = n;
                        selectScene(n, window);
                        ImGui::SaveIniSettingsToDisk(ImGui::GetIO().IniFilename);
                    }

                    // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    if (is_selected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }

            ImGui::PopItemWidth();



            ImGui::PushItemWidth(100);

            if (ImGui::InputInt("Width", &renderWidth, 10, 100))
            {
                double ratio = helpers::getRatio(renderRatio);
                renderer.initFromWidth(renderWidth, ratio);
                renderHeight = renderer.getHeight();

                glfwSetWindowSize(window, renderWidth, renderHeight);
            }

            if (ImGui::InputInt("Height", &renderHeight, 10, 100))
            {
                double ratio = helpers::getRatio(renderRatio);
                renderer.initFromHeight(renderHeight, ratio);
                renderWidth = renderer.getWidth();

                glfwSetWindowSize(window, renderWidth, renderHeight);
            }

            
            
            
            const char* combo_preview_value = renderRatios[ratio_current_idx];
            if (ImGui::BeginCombo("Aspect ratio", combo_preview_value, 0))
            {
                for (int n = 0; n < IM_ARRAYSIZE(renderRatios); n++)
                {
                    const bool is_selected = (ratio_current_idx == n);
                    if (ImGui::Selectable(renderRatios[n], is_selected))
                    {
                        ratio_current_idx = n;
                        renderRatio = renderRatios[n];

                        double current_ratio = helpers::getRatio(renderRatio);

                        if (current_ratio < 1)
                            is_ratio_landscape = false;
                        else
                            is_ratio_landscape = true;

                        if (is_ratio_landscape)
                        {
                            renderer.initFromWidth(renderWidth, current_ratio);
                            renderHeight = renderer.getHeight();
                        }
                        else
                        {
                            renderer.initFromHeight(renderHeight, current_ratio);
                            renderWidth = renderer.getWidth();
                        }

                        glfwSetWindowSize(window, renderWidth, renderHeight);
                    }

                    // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    if (is_selected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }


            ImGui::InputInt("Sample per pixel", &renderSamplePerPixel, 10, 100);

            ImGui::InputInt("Max depth", &renderMaxDepth, 10, 100);


            ImGui::PushItemWidth(150);

            std::string combo_device_preview_value = deviceModes.at(device_current_idx);
            if (ImGui::BeginCombo("CPU", combo_device_preview_value.c_str(), 0))
            {
                for (int n = 0; n < deviceModes.size(); n++)
                {
                    const bool is_selected = (device_current_idx == n);
                    if (ImGui::Selectable(deviceModes.at(n).c_str(), is_selected))
                    {
                        device_current_idx = n;
                        deviceMode = deviceModes.at(n);
                    }

                    // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    if (is_selected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }


            // antialiasing (aa sampler)
            std::string combo_antialiasing_preview_value = antialiasingModes.at(antialiasing_current_idx);
            if (ImGui::BeginCombo("Anti Aliasing", combo_antialiasing_preview_value.c_str(), 0))
            {
                for (int n = 0; n < antialiasingModes.size(); n++)
                {
                    const bool is_selected = (antialiasing_current_idx == n);
                    if (ImGui::Selectable(antialiasingModes.at(n).c_str(), is_selected))
                    {
                        antialiasing_current_idx = n;
                        antialiasingMode = antialiasingModes.at(n);
                    }

                    // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    if (is_selected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }

            // randomizer
            std::string combo_randomizer_preview_value = randomizerModes.at(randomizer_current_idx);
            if (ImGui::BeginCombo("Randomizer", combo_randomizer_preview_value.c_str(), 0))
            {
                for (int n = 0; n < randomizerModes.size(); n++)
                {
                    const bool is_selected = (randomizer_current_idx == n);
                    if (ImGui::Selectable(randomizerModes.at(n).c_str(), is_selected))
                    {
                        randomizer_current_idx = n;
                        randomizerMode = randomizerModes.at(n);
                    }

                    // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    if (is_selected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }



            ImGui::PopItemWidth();


            ImGui::PushStyleColor(ImGuiCol_Border,ImVec4(0.0f, 0.0f, 0.0f, 0.0f));


            const ImVec4 gray(0.882f, 0.882f, 0.882f, 1.0f);
            const ImVec4 blue(0.149f, 0.502f, 0.922f, 1.0f);
            const ImVec4 white(1.0f, 1.0f, 1.0f, 1.0f);

            ImGuiTogglePalette material_palette_on;
            material_palette_on.Frame = white;
            material_palette_on.Knob = blue;
            material_palette_on.KnobHover = blue;
            material_palette_on.FrameBorder = gray;

            ImGuiTogglePalette material_palette_off;
            material_palette_off.Frame = white;
            material_palette_off.Knob = gray;
            material_palette_off.KnobHover = blue;
            material_palette_off.FrameBorder = gray;

            ImGuiToggleConfig config;
            config.Flags |= ImGuiToggleFlags_Bordered | ImGuiToggleFlags_Animated;
            config.Size = ImVec2(30.0f, 18.0f);
            config.On.Palette = &material_palette_on;
            config.Off.Palette = &material_palette_off;

            
            ImGui::Toggle("Use gamma correction", &renderUseGammaCorrection, config);
            ImGui::Toggle("Auto denoiser", &renderAutoDenoise, config);


            auto windowWidth = ImGui::GetWindowSize().x;
            auto buttonWidth = ImGui::GetWindowSize().x * 0.5f;
            ImGui::SetCursorPosX((windowWidth - buttonWidth) * 0.5f);

            if (!isRenderable)
                ImGui::BeginDisabled();

            if (ImGui::GradientButton(isRendering ? "Stop" : "Render", ImVec2(buttonWidth, 50.0f),
                IM_COL32(255, 255, 255, 255), IM_COL32(102, 166, 243, 255), IM_COL32(38, 128, 235, 255)))
            {
                isCanceled = false;
                isRendering = !isRendering;

                if (isRendering)
                {
                    renderStatus = "In progress...";

                    _textBuffer.clear();

                    // render image
                    renderer.initFromWidth((unsigned int)renderWidth, helpers::getRatio(renderRatio));

                    runRaytracer("CortexRTCore.exe",
                        std::format("-quiet -width {} -height {} -ratio {} -spp {} -maxdepth {} -aa {} -random {} -gamma {} -scene \"{}\" -mode {} -save \"{}\"",
                        renderWidth,
                        renderHeight,
                        renderRatio,
                        renderSamplePerPixel,
                        renderMaxDepth,
                        antialiasing_current_idx + 1,
                        randomizer_current_idx + 1,
                        renderUseGammaCorrection ? 1 : 0,
                        sceneName,
                        device_current_idx + 1,
                        saveFilePath));
                }
                else
                {
                    renderTimer.stop();
                    renderTimer.reset();

                    renderStatus = "Cancelled";
                    renderProgress = 0.0;
                    
                    // cancel rendering
                    cancelRendering();
                }
            }

            if (!isRenderable)
                ImGui::EndDisabled();

            ImGui::PopStyleColor(1);


            


            ImGui::GradientProgressBar(renderProgress, ImVec2(-1, 0), IM_COL32(255, 255, 255, 255), IM_COL32(255, 166, 243, 255), IM_COL32(38, 128, 235, 255));

            ImGui::LabelText("Status", renderStatus);
            ImGui::LabelText("Elapsed time", renderTimer.display_time().c_str());

            // calculate remaining time
            if (renderProgress > 0 && renderProgress < 100)
            {
                unsigned int renderedLines = renderer.getRenderedLines();
                double currentTimeElapsed = renderTimer.elapsedMilliseconds();

                double averageTimePerLineMs = currentTimeElapsed / renderedLines;

                unsigned int remainingLines = renderer.getRemainingLines();
                averageRemaingTimeMs = remainingLines * averageTimePerLineMs;
            }

            ImGui::LabelText("Remaining time", timer::format_duration(averageRemaingTimeMs).c_str());

            //ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            ImGui::End();
        }





        
        ImGui::SetNextWindowSize(ImVec2(640, 200), ImGuiCond_FirstUseEver);

        if (show_scenes_manager)
        {
            // Create logs window
            bool sceneManagerOpened = true;
            ImGui::Begin("Logs", &sceneManagerOpened);

            ImGui::PushItemWidth(-1);
            
            ImGui::TextUnformatted(_textBuffer.begin());
            if (_scrollToBottom) {
                ImGui::SetScrollHereY(1.0f);
                _scrollToBottom = false;
            }

            ImGui::PopItemWidth();

            ImGui::End();
        }




        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);



        renderProgress = renderer.getRenderProgress();

        auto frameBuffer = renderer.getFrameBuffer().get();
        auto frameBufferSize = renderer.getFrameBufferSize();
        if (frameBuffer == nullptr || frameBufferSize <= 0)
        {
            // Skip drawing if the framebuffer is invalid
            std::cerr << "Invalid framebuffer!" << std::endl;
        }
        else
        {
            glDrawPixels(renderWidth, renderHeight, GL_RGBA, GL_UNSIGNED_BYTE, frameBuffer);
        }


        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Update and Render additional Platform Windows
        // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
        // For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return S_OK;
}
