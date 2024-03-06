// Dear ImGui: standalone example application for GLFW + OpenGL 3, using programmable pipeline
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)

// Learn about Dear ImGui:
// - FAQ                  https://dearimgui.com/faq
// - Getting Started      https://dearimgui.com/getting-started
// - Documentation        https://dearimgui.com/docs (same as your local docs/ folder).
// - Introduction, links and more at the top of imgui.cpp

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_spectrum.h"
#include "plotPixel.h"
#include "renderManager.h"
#include "timer.h"

#include <shlobj.h>
#include <shlwapi.h>
#include <objbase.h>





#include <stdio.h>
#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

#include <iostream>
#include <string>
#include <filesystem>
#include <map>
#include <cmath>
#include <chrono>



// for simplicity 
using namespace std;
using namespace std::filesystem;
using namespace std::chrono;



//typedef high_resolution_clock Clock;
//typedef Clock::time_point ClockTime;




// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}


int renderWidth = 512;
int renderHeight = 288;
const char* renderRatio = NULL;
const char* renderStatus = "Idle";
float renderProgress = 0.0;


#define BUFSIZE 1
HANDLE m_hChildStd_OUT_Rd = NULL;
HANDLE m_hChildStd_OUT_Wr = NULL;
HANDLE m_hreadDataFromExtProgram = NULL;
HANDLE m_render = NULL;

renderManager renderer;

timer renderTimer;

//string getExecutionTime(time_point<system_clock> start_time, time_point<system_clock> end_time)
//{
//    auto execution_time_sec = duration_cast<seconds>(end_time - start_time).count();
//    auto execution_time_min = duration_cast<minutes>(end_time - start_time).count();
//    auto execution_time_hour = duration_cast<hours>(end_time - start_time).count();
//
//    string s = "";
//
//    if (execution_time_hour > 0)
//    {
//        s.append(to_string(execution_time_hour));
//        s.append(" h, ");
//    }
//
//    if (execution_time_min > 0)
//    {
//        s.append(to_string(execution_time_min % 60));
//        s.append(" mn, ");
//    }
//
//    if (execution_time_sec > 0)
//    {
//        s.append(to_string(execution_time_sec % 60));
//        s.append(" s");
//    }
//
//    return s;
//}

string format_duration(double dms)
{
    std::chrono::duration<double, std::milli> ms{ dms };

    auto secs = duration_cast<seconds>(ms);
    auto mins = duration_cast<minutes>(secs);
    secs -= duration_cast<seconds>(mins);
    auto hour = duration_cast<hours>(mins);
    mins -= duration_cast<minutes>(hour);

    std::stringstream ss;
    ss << hour.count() << "h " << mins.count() << "mn " << secs.count() << "s";
    return ss.str();
}


void __stdcall renderasync(unsigned int* lineIndex)
{
    renderer.renderLine(*lineIndex);
}

DWORD __stdcall readDataFromExtProgram(void* argh)
{
    DWORD dwRead;
    CHAR chBuf[BUFSIZE];
    BOOL bSuccess = FALSE;

    string data = string();

    unsigned int indexPixel = 0;
    unsigned int indexLine = 0;

    unsigned int pack = 0;


    // Start measuring time
    renderTimer.start();

    for (;;)
    {
        bSuccess = ReadFile(m_hChildStd_OUT_Rd, chBuf, BUFSIZE, &dwRead, NULL);
        if (!bSuccess || dwRead == 0)
        {
            continue;
        }

        data.append(&chBuf[0], dwRead);

        if (data.ends_with("\r\n"))
        {
            string cleanedData = data.erase(data.size() - 2);
            plotPixel* plotPixel = renderer.parsePixelEntry(indexPixel, cleanedData);
            if (plotPixel)
            {
                renderer.addPixel(indexPixel, plotPixel);
                indexPixel++;
            }

            // wait a full line to be calculated before displaying it to screen
            if (pack >= renderer.getWidth())
            {
                m_render = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)renderasync, &indexLine, 0, NULL);
                WaitForSingleObject(m_render, INFINITE);

                pack = 0;
                indexLine++;
            }

            pack++;

            data.clear();

            if (indexPixel > (renderWidth * renderHeight) - 1)
            {
                // Stop measuring time
                renderTimer.stop();

                renderStatus = "Idle";
                renderProgress = 0;
            }
        }

        if (!bSuccess)
        {
            break;
        }
    }

    return 0;
}


/// <summary>
/// https://stackoverflow.com/questions/42402673/createprocess-and-capture-stdout
/// </summary>
/// <param name="externalProgram"></param>
/// <param name="arguments"></param>
/// <returns></returns>
HRESULT RunExternalProgram(string externalProgram, string arguments)
{
    path dir(current_path());
    path file(externalProgram);
    path fullexternalProgramPath = dir / file;


    if (!exists(fullexternalProgramPath))
    {
        ::MessageBox(0, "Renderer exe not found !", "TEST", MB_OK);
        return 0;
    }


    // move outside !!!!!!!!!!!!!!
    HANDLE ghJob = CreateJobObject(NULL, NULL); // GLOBAL
    if (ghJob == NULL)
    {
        ::MessageBox(0, "Could not create job object", "TEST", MB_OK);
    }
    else
    {
        JOBOBJECT_EXTENDED_LIMIT_INFORMATION jeli = { 0 };

        // Configure all child processes associated with the job to terminate when the
        jeli.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
        if (0 == SetInformationJobObject(ghJob, JobObjectExtendedLimitInformation, &jeli, sizeof(jeli)))
        {
            ::MessageBox(0, "Could not SetInformationJobObject", "TEST", MB_OK);
        }
    }


    STARTUPINFO si;
    PROCESS_INFORMATION pi;
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
        &pi)                            // Pointer to PROCESS_INFORMATION structure
        )
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }
    else
    {
        if (ghJob)
        {
            if (0 == AssignProcessToJobObject(ghJob, pi.hProcess))
            {
                ::MessageBox(0, "Could not AssignProcessToObject", "TEST", MB_OK);
            }
        }

        m_hreadDataFromExtProgram = CreateThread(0, 0, readDataFromExtProgram, NULL, 0, NULL);
    }

    return S_OK;
}

double getRatio(const char* value)
{
    double p1 = 0, p2 = 0;

    stringstream test(value);
    string segment;

    unsigned int loop = 0;
    while (getline(test, segment, ':'))
    {
        if (loop == 0)
        {
            p1 = stoul(segment, 0, 10);
        }
        else if (loop == 1)
        {
            p2 = stoul(segment, 0, 10);
        }

        loop++;
    }

    if (p1 > 0 && p2 > 0)
    {
        return p1 / p2;
    }

    return 0.0;
}




// Main code
int main(int, char**)
{
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

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
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
    //io.ConfigViewportsNoAutoMerge = true;
    io.ConfigViewportsNoTaskBarIcon = true;

    // Setup Dear ImGui style
    //ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 8.0f;
        style.ChildRounding = 8.0f;
        style.TabRounding = 8.f;
        style.FrameRounding = 8.f;
        style.GrabRounding = 8.f;
        style.PopupRounding = 8.f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

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
    io.Fonts->AddFontFromFileTTF("AdobeCleanRegular.otf", 18.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != nullptr);

    // Our state
    //bool show_demo_window = true;
    //bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.15f, 0.55f, 0.60f, 1.00f);




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

        ImGui::Spectrum::StyleColorsSpectrum();

        //https://github.com/ocornut/imgui/blob/master/docs/FONTS.md
        //ImGui::Spectrum::LoadFont(20);

        {
            // Create a window
            ImGui::Begin("Rendering parameters");

            ImGui::PushItemWidth(100);

            if (ImGui::InputInt("Width", &renderWidth, 10, 100))
            {
                renderer.initFromWidth(renderWidth, getRatio(renderRatio));
                renderHeight = renderer.getHeight();
                glfwSetWindowSize(window, renderWidth, renderHeight);
            }

            if (ImGui::InputInt("Height", &renderHeight, 10, 100))
            {
                renderer.initFromHeight(renderWidth, getRatio(renderRatio));
                renderWidth = renderer.getWidth();
                glfwSetWindowSize(window, renderWidth, renderHeight);
            }

            
            const char* items[] = { "16:9", "4:3", "3:2", "1:1" };
            renderRatio = items[0];
            static int item_current_idx = 0;
            const char* combo_preview_value = items[item_current_idx];
            if (ImGui::BeginCombo("Aspect ratio", combo_preview_value, 0))
            {
                for (int n = 0; n < IM_ARRAYSIZE(items); n++)
                {
                    const bool is_selected = (item_current_idx == n);
                    if (ImGui::Selectable(items[n], is_selected))
                    {
                        item_current_idx = n;
                        renderRatio = items[n];
                        renderer.initFromWidth(renderWidth, getRatio(renderRatio));
                        renderHeight = renderer.getHeight();
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


            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 1.0f, 1.0f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 0.0f, 1.0f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.6f, 0.2f, 1.0f));
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 28.0);
            if (ImGui::Button("Render", ImVec2(ImGui::GetWindowSize().x * 0.5f, 50.0f)))
            {
                renderStatus = "In progress...";

                

                // render image
                renderer.initFromWidth((unsigned int)renderWidth, getRatio(renderRatio));
                RunExternalProgram("MyOwnRaytracer.exe", std::format("-quiet -width {} -ratio {}", renderWidth, renderRatio));
            }
            ImGui::PopStyleColor(3);



            ImGui::LabelText("Status", renderStatus);


            ImGui::ProgressBar(renderProgress, ImVec2(-1, 0));

            string www = format_duration(renderTimer.elapsedMilliseconds());
            ImGui::LabelText("Time", www.c_str());



            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            ImGui::End();
        }


        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);

        //renderer.render();
        renderProgress = renderer.getRenderProgress();
        if (renderer.getFrameBufferSize() > 0)
        {
            glDrawPixels(renderWidth, renderHeight, GL_RGBA, GL_UNSIGNED_BYTE, renderer.getFrameBuffer());
        }



        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Update and Render additional Platform Windows
        // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
        //  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
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

    return 0;
}

