#include "effect.h"




#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <vector>


// Helper: Create framebuffer
GLuint effect::createFramebuffer(int width, int height, GLuint& textureColorBuffer)
{
    GLuint framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    // Generate texture to attach to the framebuffer
    glGenTextures(1, &textureColorBuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Attach texture to the framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorBuffer, 0);

    // Create renderbuffer for depth and stencil
    GLuint rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    // Check framebuffer completeness
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Framebuffer is incomplete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return 0;
    }

    // Unbind framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return framebuffer;
}



void effect::loadImageToTexture(const char* filepath, int& imgWidth, int& imgHeight, int& imgChannels)
{
    unsigned char* data = stbi_load(filepath, &imgWidth, &imgHeight, &imgChannels, 0);
    if (!data) {
        std::cerr << "[ERROR] Failed to load image to denoise " << filepath << " : " << stbi_failure_reason() << std::endl;
        exit(EXIT_FAILURE);
    }

    std::cout << "[INFO] Image to post process loaded successfully: width = " << imgWidth << ", height = " << imgHeight << ", channels = " << imgChannels << std::endl;

    glGenTextures(1, &m_inputTexture);
    glBindTexture(GL_TEXTURE_2D, m_inputTexture);

    // Upload image data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imgWidth, imgHeight, 0, imgChannels == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data);

    //glGenerateMipmap(GL_TEXTURE_2D);

    // Texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
}




GLuint effect::loadShader(const std::string& vertexCode, const std::string& fragmentCode)
{
    // vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const char* vertexSource = vertexCode.c_str();
    glShaderSource(vertexShader, 1, &vertexSource, nullptr);
    glCompileShader(vertexShader);
    checkCompileErrors(vertexShader, "VERTEX");

    // pixel shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const char* fragmentSource = fragmentCode.c_str();
    glShaderSource(fragmentShader, 1, &fragmentSource, nullptr);
    glCompileShader(fragmentShader);
    checkCompileErrors(fragmentShader, "FRAGMENT");

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    checkCompileErrors(shaderProgram, "PROGRAM");

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}


void effect::checkCompileErrors(GLuint shader, std::string type)
{
    GLint success;
    GLchar infoLog[1024];
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
            std::cerr << "Shader Compilation Error (" << type << "):\n" << infoLog << std::endl;
        }
    }
    else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
            std::cerr << "Program Linking Error (" << type << "):\n" << infoLog << std::endl;
        }
    }
}

GLuint effect::apply(const std::string& inputPath, const std::string& outputPath, int width, int height, pmap params)
{
    return 0;
}


// Helper: Save framebuffer content to an image
void effect::saveFramebufferToImage(GLuint framebuffer, const char* filepath, int width, int height)
{
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    std::vector<unsigned char> pixels(width * height * 3);
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());

    //stbi_flip_vertically_on_write(true); // Flip vertically for correct orientation
    if (!stbi_write_png(filepath, width, height, 3, pixels.data(), width * 3)) {
        std::cerr << "[ERROR] Failed to save post processed image." << std::endl;
    }
    else {
        std::cout << "[INFO] Post processed image saved successfully." << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind
}


effect::~effect()
{
    glDeleteTextures(1, &m_inputTexture);
}