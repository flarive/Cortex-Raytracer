#pragma once

#include <GL/glew.h>

#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

#include "../shaders.h"

#include <iostream>


class effect
{
public:
    effect() = default;

    virtual ~effect();

    virtual GLuint apply(const std::string& inputPath, const std::string& outputPath, int width, int height, float radius);

    GLuint createFramebuffer(int width, int height, GLuint& textureColorBuffer);
    void loadImageToTexture(const char* filepath, int& imgWidth, int& imgHeight, int& imgChannels);

    void checkCompileErrors(GLuint shader, std::string type);
    GLuint loadShader(const std::string& vertexCode, const std::string& fragmentCode);

    void saveFramebufferToImage(GLuint framebuffer, const char* filepath, int width, int height);

protected:

    // OpenGL texture ID for input image
    GLuint m_inputTexture;
};