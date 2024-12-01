#include "../imgui.h"
#include "../imgui_internal.h"
#include "../backends/imgui_impl_glfw.h"
#include "../backends/imgui_impl_opengl3.h"
#include "themes/imgui_spectrum.h"

#include "managers/renderManager.h"
#include "managers/sceneManager.h"
#include "managers/denoiserManager.h"
#include "managers/postProcessingManager.h"
#include "utilities/timer.h"
#include "utilities/helpers.h"
#include "misc/sceneSettings.h"
#include "misc/renderState.h"
#include "resource.h"
#include "widgets/toggle/imgui_toggle.h"
#include "widgets/toggle/imgui_toggle_palette.h"
#include "widgets/tabbar/tabbar.h"

#include "../../postprocess/effects.h" // cross project header

#include <shlobj.h>  // for get known folders
#include <windows.h>
#include <direct.h>
#include <tchar.h>




// Merge icons into default tool font
#include "widgets/icon/IconsForkAwesome.h"

#include "widgets/filedialog/ImGuiFileDialog.h"

#include <iostream>
#include <string>
#include <filesystem>
#include <chrono>
#include <thread>
#include <unordered_map>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h> // Will drag system OpenGL headers


// for simplicity 
using namespace std;
using namespace std::filesystem;
using namespace std::chrono;




// Structure to hold user data settings
struct UserData
{
    std::unordered_map<std::string, std::string> settings;
};


static UserData userData; // Static instance to store settings during session



int renderWidth = 512;
int renderHeight = 288;

static const char* renderRatios[] = { "32:9", "16:9", "4:3", "3:2", "1:1", "2:3", "3:4", "9:16", "9:32" };
std::string renderRatio = renderRatios[0];
static int ratio_current_idx = 0;

std::vector<std::string> deviceModes{};
static std::string deviceMode;
static int device_current_idx = 0;

std::vector<std::string> antialiasingModes{ "None", "Random (fast)", "MultiSamping (slower)" };
static std::string antialiasingMode;
static int antialiasing_current_idx = 0;

std::vector<std::string> postProcessEffects{};
short postProcessEffectIndex = 0;
std::string postProcessEffectArgs;
pmap postProcessEffectParams;


bool is_ratio_landscape = true;
int renderSamplePerPixel = 100;
int renderMaxDepth = 100;

bool renderUseGammaCorrection = true;
bool renderAutoDenoise = true;
bool renderPostProcessing = true;

std::string saveFilePath;
std::string saveDenoisedFilePath;

static bool _scrollToBottom = false;
static ImGuiTextBuffer _textBuffer;


static char latestDirectorySelected[255] = ".";
static std::string latestSceneSelected = "";


#define BUFSIZE_NAMED_PIPES 24
#define BUFSIZE_STANDARD_OUTPUT 27
HANDLE m_hChildStd_OUT_Rd = NULL;
HANDLE m_hChildStd_OUT_Wr = NULL;

HANDLE m_hChildStd_OUT_Rd2 = NULL;
HANDLE m_hChildStd_OUT_Wr2 = NULL;

HANDLE m_readStandardOutputRaytracerThread = NULL;
HANDLE m_readNamedPipesThread = NULL;
HANDLE m_renderRaytracerThread = NULL;

renderManager renderer;
sceneManager manager;

timer renderTimer;
double averageRemaingTimeMs = 0;

PROCESS_INFORMATION pi;
HANDLE ghJob = NULL;

HRESULT runRaytracer(string externalProgram, string arguments);


TCHAR g_szDrvMsg[] = _T("A:\\");


GLFWwindow* window;






static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}




void cancelRendering()
{
    //signal all threads to exit
    renderer.isCanceled = true;

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
    if (renderer.isCanceled)
        return S_FALSE;

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
        if (renderer.isCanceled)
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


        if (renderer.isRendering && (int)indexPixel > (renderWidth * renderHeight))
        {
            // Stop measuring time
            renderTimer.stop();

            renderer.clearFrameBuffer(false);

            renderer.renderStatus = renderState::Idle;
            renderer.renderProgress = 0.0f;
            averageRemaingTimeMs = 0;
            renderer.isRendering = false;
        }

        if (!bSuccess)
            break;
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
        if (renderer.isCanceled)
            return S_FALSE;

        bSuccess = ReadFile(m_hChildStd_OUT_Rd, chBuf, 1, &dwRead, NULL);
        if (!bSuccess || dwRead == 0)
            continue;

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
                    std::string outputPath1(saveFilePath);
                    std::string outputPath2(saveFilePath);

                    // apply denoiser if needed
                    if (renderAutoDenoise)
                    {
                        // apply denoise
                        renderer.renderStatus = renderState::Denoising;

                        _textBuffer.appendf("[INFO] Calling denoiser\n");
                        _scrollToBottom = true;

                        outputPath1 = std::string(saveFilePath).replace(saveFilePath.size() - 4, 1, "_denoised.");

                        denoiserManager denoiser(renderer, renderWidth, renderHeight);
                        if (FAILED(denoiser.runDenoiser("CortexRTDenoiser.exe", std::format("-quiet -input {} -output {} -hdr {}", saveFilePath, outputPath1, 0), outputPath1)))
                        {
                            _textBuffer.appendf("[ERROR] CortexRTDenoiser.exe not found !\n");
                            _scrollToBottom = true;
                        }
                    }

                    // apply post processing if needed
                    if (renderPostProcessing && postProcessEffectIndex > 0)
                    {
                        // apply post process
                        renderer.renderStatus = renderState::PostProcessing;

                        _textBuffer.appendf("[INFO] Calling post processor\n");
                        _scrollToBottom = true;

                        outputPath2 = std::string(outputPath1).replace(outputPath1.size() - 4, 1, "_fx.");

                        postProcessingManager postProcessor(renderer, renderWidth, renderHeight);
                        if (FAILED(postProcessor.runPostProcessor("CortexRTPostProcess.exe", std::format("-quiet -input {} -output {} -effect {} {}", outputPath1, outputPath2, postProcessEffectIndex, postProcessEffectArgs), outputPath2)))
                        {
                            _textBuffer.appendf("[ERROR] CortexRTPostProcess.exe not found !\n");
                            _scrollToBottom = true;
                        }
                    }
                }
                
                _textBuffer.appendf(data.c_str());
                _scrollToBottom = true;
            }

            data.clear();
        }

        if (!bSuccess)
            break;
    }

    return S_OK;
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

void selectScene(std::string sceneDirPath, std::string sceneFullPath)
{
    const path path = sceneFullPath;
    glfwSetWindowTitle(window, path.filename().string().c_str());

    latestSceneSelected = path.string();

    strcpy(latestDirectorySelected, sceneDirPath.c_str());


    std::unique_ptr<sceneSettings> settings = manager.readSceneSettings(path.string());
    if (settings)
    {
        renderWidth = settings->width;
        renderHeight = settings->height;
        renderRatio = settings->aspectRatio;
        renderMaxDepth = settings->depth;
        renderSamplePerPixel = settings->spp;
        saveFilePath = settings->outputFilePath;

        postProcessEffectIndex = settings->fx_index;
        renderPostProcessing = postProcessEffectIndex > 0;
        postProcessEffectArgs = settings->fx_args;
        postProcessEffectParams = settings->fx_params;

        // bof
        manager.getPostProcessBloomEffectValues(postProcessEffectParams, postProcessEffectIndex, pp_effect_bloom::threshold, pp_effect_bloom::radius, pp_effect_bloom::intensity, pp_effect_bloom::max_bloom);

        manager.getPostProcessContrastSaturationBrightnessEffectValues(postProcessEffectParams, postProcessEffectIndex, pp_effect_csb::contrast, pp_effect_csb::saturation, pp_effect_csb::brightness);



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

    renderer.isRenderable = sceneFullPath.length() > 0;
}

static void* UserData_ReadOpen(ImGuiContext*, ImGuiSettingsHandler*, const char* name)
{
    if (name != nullptr)
    {
        userData.settings[name] = ""; // Initialize with an empty string
        return &userData.settings[name]; // Return a pointer to the value
    }

    return nullptr;
}

static void UserData_ReadLine(ImGuiContext*, ImGuiSettingsHandler*, void* entry, const char* line)
{
    auto* value = static_cast<std::string*>(entry);
    if (value)
    {
        *value = line; // Update the value with the line content

        std::string str_line(line);

        std::string str_key1 = "LastSelectedScene";
        std::string str_key2 = "LastSelectedDirectory";

        if (str_line.starts_with(str_key1))
        {
            latestSceneSelected = str_line.substr(str_key1.length() + 1, str_line.length() - str_key1.length());
            if (!latestSceneSelected.empty())
            {
                selectScene(latestDirectorySelected, latestSceneSelected);
            }
        }
        else if (str_line.starts_with(str_key2))
        {
            strcpy(latestDirectorySelected, str_line.substr(str_key2.length() + 1, str_line.length() - str_key2.length()).c_str());
        }
    }
}

static void UserData_WriteAll(ImGuiContext* ctx, ImGuiSettingsHandler* handler, ImGuiTextBuffer* buf)
{
    UNREFERENCED_PARAMETER(ctx);
    UNREFERENCED_PARAMETER(handler);

    buf->appendf("[%s][%s]\n", "UserData", "Path");
    buf->appendf("LastSelectedScene=%s\n", latestSceneSelected.c_str());
    buf->appendf("LastSelectedDirectory=%s\n", latestDirectorySelected);
    buf->append("\n");


    //auto* userData = static_cast<UserData*>(handler->UserData);
    //if (userData)
    //{
    //    for (const auto& [key, value] : userData->settings)
    //    {
    //        buf->appendf("[%s]\n", key.c_str());
    //        buf->appendf("%s\n", value.c_str());
    //    }
    //}
}

void initEffects()
{
    postProcessEffects.clear();

    // fill effects vector from enum values
    for (int fxi = pp_effect::none; fxi != pp_effect::floydsteinberg; fxi++)
    {
        pp_effect fx = static_cast<pp_effect>(fxi);
        postProcessEffects.emplace_back(to_string(fx));
    }
}

void initDeviceMode()
{
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
}

void initFileDialog()
{
    ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByExtention, ".scene", ImGui::GetStyleColorVec4(ImGuiCol_Text), " " ICON_FK_FILE_O " ");

    const char* group_name2 = ICON_FK_ADDRESS_BOOK_O " Places";
    ImGuiFileDialog::Instance()->AddPlacesGroup(group_name2, 2, false, false);
    auto places_ptr2 = ImGuiFileDialog::Instance()->GetPlacesGroupPtr(group_name2);
    if (places_ptr2 != nullptr) {

        auto addKnownFolderAsPlace = [&](REFKNOWNFOLDERID knownFolder, std::string folderLabel, std::string folderIcon)
            {
                PWSTR path = NULL;
                HRESULT hr = SHGetKnownFolderPath(knownFolder, 0, NULL, &path);
                if (SUCCEEDED(hr)) {
                    IGFD::FileStyle style;
                    style.icon = std::format("   {}", folderIcon);
                    auto place_path = IGFD::Utils::UTF8Encode(path);
                    places_ptr2->AddPlace(folderLabel, place_path, false, style);
                }
                CoTaskMemFree(path);
            };

        addKnownFolderAsPlace(FOLDERID_Desktop, "Desktop", "   " ICON_FK_DESKTOP);
        addKnownFolderAsPlace(FOLDERID_Downloads, "Downloads", "   " ICON_FK_DOWNLOAD);
        addKnownFolderAsPlace(FOLDERID_Pictures, "Pictures", "   " ICON_FK_PICTURE_O);
        addKnownFolderAsPlace(FOLDERID_Music, "Music", "   " ICON_FK_MUSIC);
        addKnownFolderAsPlace(FOLDERID_Videos, "Videos", "   " ICON_FK_FILM);
    }

    ImGuiFileDialog::Instance()->AddPlacesGroup(placesDevicesGroupName, 3, false, true);
    auto places_ptr3 = ImGuiFileDialog::Instance()->GetPlacesGroupPtr(placesDevicesGroupName);
    if (places_ptr3 != nullptr) {
        auto addLogicalDriveAsPlace = [&](std::string logicalDrive, std::string logicalDriveName)
            {
                IGFD::FileStyle style;
                style.icon = "      " ICON_FK_HDD_O;
                places_ptr3->AddPlace(std::format("{} ({})", logicalDriveName, logicalDrive), logicalDrive, false, style);
            };


        ULONG uDriveMask = _getdrives();
        if (uDriveMask > 0)
        {
            while (uDriveMask) {
                if (uDriveMask & 1)
                {
                    CHAR szVolumeName[255];
                    if (GetVolumeInformationA(g_szDrvMsg, szVolumeName, MAX_PATH, NULL, NULL, NULL, NULL, 0))
                        addLogicalDriveAsPlace(std::string(g_szDrvMsg), std::string(szVolumeName));
                }

                ++g_szDrvMsg[0];
                uDriveMask >>= 1;
            }
        }
    }
}

ImGuiToggleConfig getToogleConfig()
{
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

    ImGuiToggleConfig toggle_config;
    toggle_config.Flags |= ImGuiToggleFlags_Bordered | ImGuiToggleFlags_Animated;
    toggle_config.Size = ImVec2(30.0f, 18.0f);
    toggle_config.On.Palette = &material_palette_on;
    toggle_config.Off.Palette = &material_palette_off;

    return toggle_config;
}

void textCentered(std::string text)
{
    auto windowWidth = ImGui::GetWindowSize().x;
    auto textWidth = ImGui::CalcTextSize(text.c_str()).x;

    ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
    ImGui::Text(text.c_str());
}

void renderFxParams()
{
    ImGui::BeginChild("EffectsArgsRegion", ImVec2(0, 0), ImGuiChildFlags_None, ImGuiWindowFlags_NoScrollbar);

    if (postProcessEffectIndex == pp_effect::bloom)
    {
        ImGui::SliderFloat("Threshold", &pp_effect_bloom::threshold, 0.0f, 10.0f, "%.2f", ImGuiSliderFlags_None);
        ImGui::SliderFloat("Radius", &pp_effect_bloom::radius, 0.0f, 500.0f, "%.2f", ImGuiSliderFlags_None);
        ImGui::SliderFloat("Intensity", &pp_effect_bloom::intensity, 0.0f, 10.0f, "%.2f", ImGuiSliderFlags_None);
        ImGui::SliderFloat("Max bloom", &pp_effect_bloom::max_bloom, 0.0f, 10.0f, "%.2f", ImGuiSliderFlags_None);
    }
    else if (postProcessEffectIndex == pp_effect::csb)
    {
        ImGui::SliderFloat("Contrast", &pp_effect_csb::contrast, 0.0f, 5.0f, "%.2f", ImGuiSliderFlags_None);
        ImGui::SliderFloat("Saturation", &pp_effect_csb::saturation, 0.0f, 5.0f, "%.2f", ImGuiSliderFlags_None);
        ImGui::SliderFloat("Brightness", &pp_effect_csb::brightness, 0.0f, 5.0f, "%.2f", ImGuiSliderFlags_None);
    }
    else if (postProcessEffectIndex == pp_effect::floydsteinberg)
    {
    }

    ImGui::EndChild();
}


void renderHeader()
{
    // Scenes directory picker
    ImGui::PushItemWidth(242);
    ImGui::InputTextWithHint("##", "Choose a scene", latestDirectorySelected, IM_ARRAYSIZE(latestDirectorySelected));

    ImGui::SameLine();

    if (ImGui::Button(ICON_FK_FOLDER_OPEN_O, ImVec2(34, 24))) {
        IGFD::FileDialogConfig fdconfig;
        fdconfig.path = std::string(latestDirectorySelected);// ".";
        fdconfig.countSelectionMax = 1;
        fdconfig.sidePaneWidth = 280.0f;
        fdconfig.flags = ImGuiFileDialogFlags_Modal | ImGuiFileDialogFlags_ShowDevicesButton;

        ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", ICON_FK_FOLDER_OPEN_O " Choose a scene directory", ".scene", fdconfig);
    }

    ImGui::PushItemWidth(-1);

    std::filesystem::path p = path(latestSceneSelected);
    textCentered(p.filename().string());

    // display file dialog popup window
    ImVec2 maxSize = ImVec2(1000, 800);  // The full display area
    ImVec2 minSize = ImVec2(800, 400);  // Half the display area
    if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey", ImGuiWindowFlags_None, minSize, maxSize))
    {
        if (ImGuiFileDialog::Instance()->IsOk())
        {
            // action if OK
            std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
            std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();

            selectScene(filePath, filePathName);
            ImGui::SaveIniSettingsToDisk(ImGui::GetIO().IniFilename);
        }

        // close
        ImGuiFileDialog::Instance()->Close();
    }

    ImGui::PopItemWidth();
}


void renderTab1()
{
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
    if (ImGui::BeginCombo("Aspect ratio", combo_preview_value, ImGuiComboFlags_None))
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
}

void renderTab2()
{
    ImGui::PushItemWidth(150);

    std::string combo_device_preview_value = deviceModes.at(device_current_idx);
    if (ImGui::BeginCombo("CPU", combo_device_preview_value.c_str(), ImGuiComboFlags_None))
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
    if (ImGui::BeginCombo("Anti Aliasing", combo_antialiasing_preview_value.c_str(), ImGuiComboFlags_None))
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

    ImGui::PopItemWidth();


    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

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

    ImGuiToggleConfig toggle_config;
    toggle_config.Flags |= ImGuiToggleFlags_Bordered | ImGuiToggleFlags_Animated;
    toggle_config.Size = ImVec2(30.0f, 18.0f);
    toggle_config.On.Palette = &material_palette_on;
    toggle_config.Off.Palette = &material_palette_off;

    ImGui::Spacing();


    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

    ImGui::Toggle("Use gamma correction", &renderUseGammaCorrection, toggle_config);
    ImGui::Toggle("Auto denoiser", &renderAutoDenoise, toggle_config);
    

    ImGui::PopStyleVar();

    ImGui::PopStyleColor();
}

void renderTab3()
{
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

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

    ImGuiToggleConfig toggle_config;
    toggle_config.Flags |= ImGuiToggleFlags_Bordered | ImGuiToggleFlags_Animated;
    toggle_config.Size = ImVec2(30.0f, 18.0f);
    toggle_config.On.Palette = &material_palette_on;
    toggle_config.Off.Palette = &material_palette_off;

    ImGui::Toggle("Post processing", &renderPostProcessing, toggle_config);

    ImGui::PopStyleColor();

    // available effects
    if (!renderPostProcessing)
        ImGui::BeginDisabled();

    std::string combo_effects_preview_value = postProcessEffects.at(postProcessEffectIndex);
    if (ImGui::BeginCombo("Effects", combo_effects_preview_value.c_str(), ImGuiComboFlags_None))
    {
        for (short n = 0; n < postProcessEffects.size(); n++)
        {
            const bool is_selected = (postProcessEffectIndex == n);
            if (ImGui::Selectable(postProcessEffects.at(n).c_str(), is_selected))
            {
                postProcessEffectIndex = n;
            }

            // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
            if (is_selected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    renderFxParams();


    if (!renderPostProcessing)
        ImGui::EndDisabled();
}



void renderFooter()
{
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

    auto windowWidth = ImGui::GetWindowSize().x;
    auto buttonWidth = ImGui::GetWindowSize().x * 0.5f;
    ImGui::SetCursorPosX((windowWidth - buttonWidth) * 0.5f);

    if (!renderer.isRenderable)
        ImGui::BeginDisabled();

    if (ImGui::GradientButton(renderer.isRendering ? "Stop" : "Render", ImVec2(buttonWidth, 50.0f),
        IM_COL32(255, 255, 255, 255), IM_COL32(102, 166, 243, 255), IM_COL32(38, 128, 235, 255)))
    {
        renderer.isCanceled = false;
        renderer.isRendering = !renderer.isRendering;

        if (renderer.isRendering)
        {
            renderer.renderStatus = renderState::InProgress;

            _textBuffer.clear();

            // render image
            renderer.initFromWidth((unsigned int)renderWidth, helpers::getRatio(renderRatio));

            runRaytracer("CortexRTCore.exe",
                std::format("-quiet -width {} -height {} -ratio {} -spp {} -maxdepth {} -aa {} -gamma {} -scene \"{}\" -mode {} -save \"{}\"",
                    renderWidth,
                    renderHeight,
                    renderRatio,
                    renderSamplePerPixel,
                    renderMaxDepth,
                    antialiasing_current_idx + 1,
                    renderUseGammaCorrection ? 1 : 0,
                    latestSceneSelected,
                    device_current_idx + 1,
                    saveFilePath));
        }
        else
        {
            renderTimer.stop();
            renderTimer.reset();

            renderer.renderStatus = renderState::Cancelled;
            renderer.renderProgress = 0.0f;

            // cancel rendering
            cancelRendering();
        }
    }

    if (!renderer.isRenderable)
        ImGui::EndDisabled();

    ImGui::PopStyleColor(1);





    ImGui::GradientProgressBar(renderer.renderProgress, ImVec2(-1, 0), IM_COL32_WHITE, IM_COL32(255, 166, 243, 255), IM_COL32(38, 128, 235, 255));

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

    ImGui::LabelText("Status", to_string(renderer.renderStatus).c_str());
    ImGui::LabelText("Elapsed time", renderTimer.display_time().c_str());

    // calculate remaining time
    if (renderer.renderProgress > 0 && renderer.renderProgress < 100)
    {
        unsigned int renderedLines = renderer.getRenderedLines();
        double currentTimeElapsed = renderTimer.elapsedMilliseconds();

        double averageTimePerLineMs = currentTimeElapsed / renderedLines;

        unsigned int remainingLines = renderer.getRemainingLines();
        averageRemaingTimeMs = remainingLines * averageTimePerLineMs;
    }

    ImGui::LabelText("Remaining time", timer::format_duration(averageRemaingTimeMs).c_str());

    ImGui::PopStyleVar();
}



void renderLogsWindow()
{
    ImGui::SetNextWindowSize(ImVec2(640, 200), ImGuiCond_FirstUseEver);

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

void renderParametersWindow()
{
    ImGui::SetNextWindowSize(ImVec2(300, 600), ImGuiCond_FirstUseEver);

    // Create rendering parameters window
    bool renderingParamsOpened = true;
    ImGui::Begin("Rendering parameters", &renderingParamsOpened, ImGuiWindowFlags_NoResize);

    ImGui::PushItemWidth(-1);

    renderHeader();

    // Desired fixed tab height
    float fixed_tab_height = 28.0f;

    // Variable to track the selected tab
    static int selected_tab = 0;

    // Save the cursor position before drawing the tab bar
    ImVec2 tab_bar_start_pos = ImGui::GetCursorPos();

    // Push style to control tab height
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8.0f, (fixed_tab_height - ImGui::GetTextLineHeight()) * 0.5f));
    ImGui::PushStyleVar(ImGuiStyleVar_TabBarBorderSize, 0.0f);

    // Draw the tab bar
    if (BeginTabBar("MyTabBar", ImGuiWindowFlags_NoScrollbar))
    {
        if (BeginTabItem("Basic", ImGuiTabItemFlags_Trailing))
        {
            selected_tab = 0; // Set the selected tab
            ImGui::EndTabItem();
        }
        if (BeginTabItem("Advanced", ImGuiTabItemFlags_Trailing))
        {
            selected_tab = 1; // Set the selected tab
            ImGui::EndTabItem();
        }
        if (BeginTabItem("Post Processing", ImGuiTabItemFlags_Trailing))
        {
            selected_tab = 2; // Set the selected tab
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }



    // Pop style variable to restore the previous frame padding
    ImGui::PopStyleVar();
    ImGui::PopStyleVar();

    // Add spacing after the tab bar to separate content
    ImGui::SetCursorPosY(tab_bar_start_pos.y + fixed_tab_height + 20);

    // Draw the content area
    ImGui::BeginChild("ContentRegion", ImVec2(0, 260), ImGuiChildFlags_Border, ImGuiWindowFlags_NoScrollbar);

    // Render different content based on the selected tab
    if (selected_tab == 0)
        renderTab1();
    else if (selected_tab == 1)
        renderTab2();
    if (selected_tab == 2)
        renderTab3();

    ImGui::EndChild();


    ImGui::BeginChild("FooterRegion", ImVec2(0, 0), ImGuiChildFlags_None, ImGuiWindowFlags_NoScrollbar);
    renderFooter();
    ImGui::EndChild();



    //ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
    ImGui::End();
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
#endif

    // Create window with graphics context
    window = glfwCreateWindow(512, 288, "RayTracer", nullptr, nullptr);
    if (window == nullptr)
        return S_FALSE;

    glfwMakeContextCurrent(window);



    // Load the icon from resources
    GLFWimage icon = helpers::loadIconFromResource(PNG_ICON);

    // Set the window icon
    glfwSetWindowIcon(window, 1, &icon);

    // Free the loaded image data
    stbi_image_free(icon.pixels);

    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();


    // Read/Write custom data in imgui.ini
    ImGuiSettingsHandler ini_handler;
    ini_handler.TypeName = "UserData";
    ini_handler.TypeHash = ImHashStr("UserData");
    ini_handler.ReadOpenFn = UserData_ReadOpen;
    ini_handler.ReadLineFn = UserData_ReadLine;
    ini_handler.WriteAllFn = UserData_WriteAll;
    ImGui::AddSettingsHandler(&ini_handler);

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
    io.ConfigViewportsNoTaskBarIcon = true;

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

    // Load user settings from imgui.ini
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


    // Set icons font
    ImFontConfig config;
    config.MergeMode = true;
    config.GlyphMinAdvanceX = 13.0f; // Use if you want to make the icon monospaced
    static const ImWchar icon_ranges[] = { ICON_MIN_FK, ICON_MAX_16_FK, 0 };
    ImFont* font = io.Fonts->AddFontFromFileTTF(FONT_ICON_FILE_NAME_FK, 16.0f, &config, icon_ranges);
    IM_ASSERT(font != nullptr);


    // Our state
    //bool show_demo_window = true;
    bool show_parameters_window = true;
    bool show_logs_window = true;
    ImVec4 clear_color = ImVec4(0.80f, 0.80f, 0.80f, 1.00f);

    //initSceneSelector(window);
    initDeviceMode();
    initFileDialog();
    initEffects();

    // Main UI loop
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
        
        if (show_parameters_window)
            renderParametersWindow();

        if (show_logs_window)
            renderLogsWindow();




        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);



        renderer.renderProgress = renderer.getRenderProgress();

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
