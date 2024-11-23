#include <GL/glew.h>

//#define STB_IMAGE_IMPLEMENTATION
//#define STBI_FAILURE_USERMSG
//#include "stb_image.h"
//
//#define STB_IMAGE_WRITE_IMPLEMENTATION
//#include "stb_image_write.h"


#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h> // Will drag system OpenGL headers


#include "parameters.h"
#include "effects/effect.h"
#include "effects/bloom_effect.h"



#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <cmath>



//void saveFramebufferToImage(GLuint framebuffer, const char* filepath, int width, int height);

//// Helper: Save framebuffer content to an image
//void saveFramebufferToImage(GLuint framebuffer, const char* filepath, int width, int height)
//{
//    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
//
//    std::vector<unsigned char> pixels(width * height * 3);
//    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());
//
//    //stbi_flip_vertically_on_write(true); // Flip vertically for correct orientation
//    if (!stbi_write_png(filepath, width, height, 3, pixels.data(), width * 3)) {
//        std::cerr << "Failed to save image to " << filepath << std::endl;
//    }
//    else {
//        std::cout << "Image saved to " << filepath << std::endl;
//    }
//
//    glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind
//}




//int main_singlepass(int argc, char* argv[])
//{
//    parameters params = parameters::getArgs(argc, argv);
//
//    // Initialize GLFW
//    if (!glfwInit()) {
//        std::cerr << "Failed to initialize GLFW!" << std::endl;
//        return -1;
//    }
//
//    // Create an invisible GLFW window
//    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE); // Don't show the window
//    GLFWwindow* window = glfwCreateWindow(800, 600, "Headless Rendering", nullptr, nullptr);
//    if (!window) {
//        std::cerr << "Failed to create GLFW window!" << std::endl;
//        glfwTerminate();
//        return -1;
//    }
//    glfwMakeContextCurrent(window);
//
//    // Initialize GLEW
//    if (glewInit() != GLEW_OK) {
//        std::cerr << "Failed to initialize GLEW!" << std::endl;
//        glfwTerminate();
//        return -1;
//    }
//
//    // Set up OpenGL
//    int width = 512, height = 512;
//    glViewport(0, 0, width, height);
//
//    // Create framebuffer and texture
//    GLuint framebuffer, textureColorBuffer;
//    framebuffer = createFramebuffer(width, height, textureColorBuffer);
//
//    // Load your shader program (pseudo-code, replace with actual shader loading)
//    GLuint shaderProgram = loadShader(shaders::screen_vert_shader, shaders::brightness_frag_shader);
//
//
//
//    // Load the input image as a texture
//    //GLuint inputTexture;
//    //int imgWidth, imgHeight, imgChannels;
//    //loadImageToTexture(params.inputpath.c_str(), inputTexture, imgWidth, imgHeight, imgChannels);
//
//
//
//    // Load the input image as a texture
//    GLuint inputTexture;
//    glGenTextures(1, &inputTexture);
//    glBindTexture(GL_TEXTURE_2D, inputTexture);
//
//    int imgWidth, imgHeight, imgChannels;
//    unsigned char* imageData = stbi_load(params.inputpath.c_str(), &imgWidth, &imgHeight, &imgChannels, 0);
//    if (!imageData) {
//        std::cerr << "Failed to load input image!" << std::endl;
//        glfwTerminate();
//        return -1;
//    }
//
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imgWidth, imgHeight, 0, imgChannels == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, imageData);
//    glGenerateMipmap(GL_TEXTURE_2D);
//    stbi_image_free(imageData);
//
//
//
//
//
//    // Render to the framebuffer
//    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
//    glViewport(0, 0, width, height);
//
//    // Clear the framebuffer and render with the bloom shader
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//
//
//
//    glUseProgram(shaderProgram);
//    glBindTexture(GL_TEXTURE_2D, inputTexture);
//
//    //glUniform1i(glGetUniformLocation(shaderProgram, "horizontal"), GL_FALSE);
//    //glUniform1f(glGetUniformLocation(shaderProgram, "radius"), 100.0f);
//
//    glUniform1f(glGetUniformLocation(shaderProgram, "threshold"), 0.4f);
//
//
//
//    // Full-screen rendering (no quad required; use the default OpenGL pipeline or a full-screen shader)
//    glDrawArrays(GL_TRIANGLES, 0, 3); // Use a simple triangle covering the screen if your shader can handle it
//
//
//    // Save the framebuffer content to an image
//    saveFramebufferToImage(framebuffer, params.outputpath.c_str(), width, height);
//
//    // Cleanup
//    glDeleteTextures(1, &inputTexture);
//    glDeleteTextures(1, &textureColorBuffer);
//    glDeleteFramebuffers(1, &framebuffer);
//
//    glfwDestroyWindow(window);
//    glfwTerminate();
//    return 0;
//}



int main(int argc, char* argv[])
{
    float radius = 100.0f;

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

    
    std::shared_ptr<effect> fx = std::make_shared<bloom_effect>();


    
    // Load the input image as a texture
    fx->loadImageToTexture(params.inputpath.c_str(), imgWidth, imgHeight, imgChannels);

    // Set up OpenGL viewport
    glViewport(0, 0, imgWidth, imgHeight);


    GLuint framebuffer = fx->apply(params.inputpath.c_str(), params.outputpath.c_str(), imgWidth, imgHeight, 100.0f);
    

    //GLuint framebuffer = bloom(params.inputpath.c_str(), params.outputpath.c_str(), imgWidth, imgHeight, 100.0f);

    // Save the final output
    fx->saveFramebufferToImage(framebuffer, params.outputpath.c_str(), imgWidth, imgHeight);

    // Cleanup
    glDeleteFramebuffers(1, &framebuffer);


    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}