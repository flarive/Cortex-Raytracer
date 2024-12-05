#include <GL/glew.h>


#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

#include "effects.h"
#include "parameters.h"

#include "effects/effect.h"
#include "effects/bloom_effect.h"
#include "effects/denoise_effect.h"
#include "effects/csb_effect.h"
#include "effects/floydsteinberg_effect.h"


/// <summary>
/// https://en.wikipedia.org/wiki/Video_post-processing
/// </summary>
/// <param name="argc"></param>
/// <param name="argv"></param>
/// <returns></returns>
int main(int argc, char* argv[])
{
    parameters params = parameters::getArgs(argc, argv);

    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW!" << std::endl;
        return -1;
    }


    int imgWidth = 0, imgHeight = 0, imgChannels = 0;

    // Create an invisible GLFW window
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    GLFWwindow* window = glfwCreateWindow(64, 64, "Headless Rendering", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window!" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW!" << std::endl;
        glfwTerminate();
        return -1;
    }

    
    std::shared_ptr<effect> fx = nullptr;

    if (params.fx_index == pp_effect::bloom)
        fx = std::make_shared<bloom_effect>();
    else if (params.fx_index == pp_effect::denoise)
        fx = std::make_shared<denoise_effect>();
    else if (params.fx_index == pp_effect::csb)
        fx = std::make_shared<csb_effect>();
    else if (params.fx_index == pp_effect::floydsteinberg)
        fx = std::make_shared<floydsteinberg_effect>();
    else
        return 1;



    // Load the input image as a texture
    fx->loadImageToTexture(params.inputpath.c_str(), imgWidth, imgHeight, imgChannels);

    // Set up OpenGL viewport
    glViewport(0, 0, imgWidth, imgHeight);


    // Apply post process effect
    GLuint framebuffer = fx->apply(params.inputpath.c_str(), params.outputpath.c_str(), imgWidth, imgHeight, params.fx_args);

    // Save the final output
    fx->saveFramebufferToImage(framebuffer, params.outputpath.c_str(), imgWidth, imgHeight);

    // Cleanup
    glDeleteFramebuffers(1, &framebuffer);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}