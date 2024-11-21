#include <GL/glew.h>

#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"


#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h> // Will drag system OpenGL headers


#include "parameters.h"


#include <vector>

#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <cmath>


// Shader loading utility
GLuint loadShader(const char* vertexPath, const char* fragmentPath);
void checkCompileErrors(GLuint shader, std::string type);

// Function prototypes
GLuint createFramebuffer(int width, int height, GLuint& textureColorBuffer);
void saveFramebufferToImage(GLuint framebuffer, const char* filepath, int width, int height);
//void loadImageToTexture(const char* filepath, GLuint& texture, int& imgWidth, int& imgHeight, int& imgChannels);




// OpenGL texture ID for input image
GLuint inputTexture;


void uploadGaussianWeights(GLuint shaderProgram, int radius, float sigma)
{
    std::vector<float> weights(radius + 1); // Symmetric weights
    float sum = 0.0f;

    for (int i = 0; i <= radius; ++i) {
        weights[i] = exp(-0.5f * (i * i) / (sigma * sigma));
        sum += (i == 0 ? weights[i] : 2.0f * weights[i]); // Center weight counts once, others twice
    }

    // Normalize weights
    for (int i = 0; i <= radius; ++i) {
        weights[i] /= sum;
    }

    // Upload to shader
    glUseProgram(shaderProgram);
    glUniform1i(glGetUniformLocation(shaderProgram, "blurRadius"), radius);
    glUniform1fv(glGetUniformLocation(shaderProgram, "weights"), radius + 1, weights.data());
}

//void loadImageToTexture(const char* filepath, GLuint& texture, int& imgWidth, int& imgHeight, int& imgChannels)
//{
//    unsigned char* data = stbi_load(filepath, &imgWidth, &imgHeight, &imgChannels, 0);
//    if (!data) {
//        std::cerr << "Failed to load image: " << filepath << std::endl;
//        exit(EXIT_FAILURE);
//    }
//
//    std::cout << "Image loaded: " << imgWidth << "x" << imgHeight << " Channels: " << imgChannels << std::endl;
//
//
//    glGenTextures(1, &inputTexture);
//    glBindTexture(GL_TEXTURE_2D, inputTexture);
//
//    // Upload image data
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imgWidth, imgHeight, 0,
//        imgChannels == 4 ? GL_RGBA : GL_RGB,
//        GL_UNSIGNED_BYTE, data);
//
//    //glGenerateMipmap(GL_TEXTURE_2D);
//
//    // Texture parameters
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//
//    stbi_image_free(data);
//}

// Helper: Create framebuffer
GLuint createFramebuffer(int width, int height, GLuint& textureColorBuffer)
{
    GLuint framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    // Create a texture for color attachment
    glGenTextures(1, &textureColorBuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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

    glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind
    return framebuffer;
}

// Helper: Save framebuffer content to an image
void saveFramebufferToImage(GLuint framebuffer, const char* filepath, int width, int height)
{
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    std::vector<unsigned char> pixels(width * height * 3);
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());

    //stbi_flip_vertically_on_write(true); // Flip vertically for correct orientation
    if (!stbi_write_png(filepath, width, height, 3, pixels.data(), width * 3)) {
        std::cerr << "Failed to save image to " << filepath << std::endl;
    }
    else {
        std::cout << "Image saved to " << filepath << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind
}


//int main_work(int argc, char* argv[])
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
//    int width = 512, height = 288;
//    glViewport(0, 0, width, height);
//
//    // Create framebuffer and texture
//    GLuint framebuffer, textureColorBuffer;
//    framebuffer = createFramebuffer(width, height, textureColorBuffer);
//
//    // Load your shader program (pseudo-code, replace with actual shader loading)
//    GLuint shaderProgram = loadShader("shaders/vertex_shader.glsl", "shaders/bloom.frag");
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
//
//    glUniform1f(glGetUniformLocation(shaderProgram, "bloom_spread"), 10.0f);
//    glUniform1f(glGetUniformLocation(shaderProgram, "bloom_intensity"), 6.0f);
//
//    // Full-screen rendering (no quad required; use the default OpenGL pipeline or a full-screen shader)
//    glDrawArrays(GL_TRIANGLES, 0, 3); // Use a simple triangle covering the screen if your shader can handle it
//
//    // Save the framebuffer content to an image
//    saveFramebufferToImage(framebuffer, "e:\\output.png", width, height);
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



//int main_working(int argc, char* argv[])
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
//    int width = 512, height = 288;
//    glViewport(0, 0, width, height);
//
//    // Create framebuffer and texture
//    GLuint framebuffer, textureColorBuffer;
//    framebuffer = createFramebuffer(width, height, textureColorBuffer);
//
//    // Load your shader program (pseudo-code, replace with actual shader loading)
//    GLuint shaderProgram = loadShader("shaders/vertex_shader.glsl", "shaders/bloom.frag");
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
//
//    glUniform1f(glGetUniformLocation(shaderProgram, "bloom_spread"), 10.0f);
//    glUniform1f(glGetUniformLocation(shaderProgram, "bloom_intensity"), 6.0f);
//
//    // Full-screen rendering (no quad required; use the default OpenGL pipeline or a full-screen shader)
//    glDrawArrays(GL_TRIANGLES, 0, 3); // Use a simple triangle covering the screen if your shader can handle it
//
//    // Save the framebuffer content to an image
//    saveFramebufferToImage(framebuffer, "e:\\output.png", width, height);
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
    parameters params = parameters::getArgs(argc, argv);
    
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW!" << std::endl;
        return -1;
    }

    int imgWidth, imgHeight, imgChannels;
    unsigned char* imageData = stbi_load(params.inputpath.c_str(), &imgWidth, &imgHeight, &imgChannels, 0);
    if (!imageData) {
        std::cerr << "Failed to load input image!" << std::endl;
        glfwTerminate();
        return -1;
    }


    // Create an invisible GLFW window
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    GLFWwindow* window = glfwCreateWindow(imgWidth, imgHeight, "Headless Rendering", nullptr, nullptr);
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

    // Set up OpenGL
    glViewport(0, 0, imgWidth, imgHeight);




    // Load the input image as a texture
    //GLuint inputTexture;
    //int imgWidth, imgHeight, imgChannels;
    //loadImageToTexture(params.inputpath.c_str(), inputTexture, imgWidth, imgHeight, imgChannels);



    // Load the input image as a texture
    GLuint inputTexture;
    glGenTextures(1, &inputTexture);
    glBindTexture(GL_TEXTURE_2D, inputTexture);

    

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imgWidth, imgHeight, 0, imgChannels == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, imageData);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(imageData);







    // Framebuffers
    GLuint brightFramebuffer, horizontalFramebuffer, verticalFramebuffer;
    GLuint brightTexture, horizontalTexture, verticalTexture;

    brightFramebuffer = createFramebuffer(imgWidth, imgHeight, brightTexture);
    horizontalFramebuffer = createFramebuffer(imgWidth, imgHeight, horizontalTexture);
    verticalFramebuffer = createFramebuffer(imgWidth, imgHeight, verticalTexture);





    // Load shaders
    GLuint brightnessShader = loadShader("shaders/vertex_shader.glsl", "shaders/brightness_shader.glsl");
    GLuint blurShader = loadShader("shaders/vertex_shader.glsl", "shaders/blur_shader.glsl");
    GLuint combineShader = loadShader("shaders/vertex_shader.glsl", "shaders/combine_shader.glsl");

    // Brightness extraction pass
    glBindFramebuffer(GL_FRAMEBUFFER, brightFramebuffer);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(brightnessShader);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, inputTexture);
    glUniform1f(glGetUniformLocation(brightnessShader, "threshold"), 1.0f); // Adjust threshold
    glDrawArrays(GL_TRIANGLES, 0, 3);

    int radius = 10.0;
    float sigma = 1.0f;
    float scale = 1.0f;

    


    // Horizontal blur pass
    glBindFramebuffer(GL_FRAMEBUFFER, horizontalFramebuffer);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(blurShader);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, brightTexture);
    glUniform1i(glGetUniformLocation(blurShader, "horizontal"), GL_TRUE);
    glUniform1i(glGetUniformLocation(blurShader, "blurRadius"), radius);
    glUniform1f(glGetUniformLocation(blurShader, "blurScale"), scale);
    uploadGaussianWeights(blurShader, radius, sigma);
    glDrawArrays(GL_TRIANGLES, 0, 3);



    // Vertical blur pass
    glBindFramebuffer(GL_FRAMEBUFFER, verticalFramebuffer);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(blurShader);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, horizontalTexture);
    glUniform1i(glGetUniformLocation(blurShader, "horizontal"), GL_FALSE);
    glUniform1i(glGetUniformLocation(blurShader, "blurRadius"), radius);
    glUniform1f(glGetUniformLocation(blurShader, "blurScale"), scale);
    uploadGaussianWeights(blurShader, radius, sigma);
    glDrawArrays(GL_TRIANGLES, 0, 3);




    // Combine pass
    glBindFramebuffer(GL_FRAMEBUFFER, 0); // Render to default framebuffer
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(combineShader);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, inputTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, verticalTexture);
    glDrawArrays(GL_TRIANGLES, 0, 3);



    // Save the final output
    saveFramebufferToImage(0, "d:\\output.png", imgWidth, imgHeight);

    // Cleanup
    glDeleteTextures(1, &inputTexture);
    glDeleteTextures(1, &brightTexture);
    glDeleteTextures(1, &horizontalTexture);
    glDeleteTextures(1, &verticalTexture);
    glDeleteFramebuffers(1, &brightFramebuffer);
    glDeleteFramebuffers(1, &horizontalFramebuffer);
    glDeleteFramebuffers(1, &verticalFramebuffer);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}


GLuint loadShader(const char* vertexPath, const char* fragmentPath)
{
    // Helper function to read shader code from a file
    auto readShaderFile = [](const char* filePath) -> std::string {
        std::ifstream shaderFile(filePath);
        if (!shaderFile.is_open()) {
            std::cerr << "Failed to open shader file: " << filePath << std::endl;
            exit(EXIT_FAILURE);
        }
        std::stringstream buffer;
        buffer << shaderFile.rdbuf();
        shaderFile.close();
        return buffer.str();
        };

    // Read shader files
    std::string vertexCode = readShaderFile(vertexPath);
    std::string fragmentCode = readShaderFile(fragmentPath);

    // Compile vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const char* vertexSource = vertexCode.c_str();
    glShaderSource(vertexShader, 1, &vertexSource, nullptr);
    glCompileShader(vertexShader);
    checkCompileErrors(vertexShader, "VERTEX");

    // Compile fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const char* fragmentSource = fragmentCode.c_str();
    glShaderSource(fragmentShader, 1, &fragmentSource, nullptr);
    glCompileShader(fragmentShader);
    checkCompileErrors(fragmentShader, "FRAGMENT");

    // Link shaders into a program
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    checkCompileErrors(shaderProgram, "PROGRAM");

    // Clean up shaders as they're now linked into the program
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}


void checkCompileErrors(GLuint shader, std::string type)
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
