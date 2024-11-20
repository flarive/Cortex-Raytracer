#include <GL/glew.h>

#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"


#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h> // Will drag system OpenGL headers


#include "parameters.h"

#include <iostream>
#include <vector>

// Shader loading utility
GLuint loadShader(const char* vertexPath, const char* fragmentPath);
void checkCompileErrors(GLuint shader, std::string type);

// Function prototypes
GLuint createFramebuffer(int width, int height, GLuint& textureColorBuffer);
void saveFramebufferToImage(GLuint framebuffer, const char* filepath, int width, int height);


// Image size
int width, height, channels;

// OpenGL texture ID for input image
GLuint inputTexture;

void loadImageToTexture(const char* filepath)
{
    unsigned char* data = stbi_load(filepath, &width, &height, &channels, 0);
    if (!data) {
        std::cerr << "Failed to load image: " << filepath << std::endl;
        exit(EXIT_FAILURE);
    }

    std::cout << "Image loaded: " << width << "x" << height << " Channels: " << channels << std::endl;


    glGenTextures(1, &inputTexture);
    glBindTexture(GL_TEXTURE_2D, inputTexture);

    // Upload image data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
        channels == 4 ? GL_RGBA : GL_RGB,
        GL_UNSIGNED_BYTE, data);

    // Texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
}

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


int main(int argc, char* argv[])
{
    parameters params = parameters::getArgs(argc, argv);
 
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW!" << std::endl;
        return -1;
    }

    // Create an invisible GLFW window
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE); // Don't show the window
    GLFWwindow* window = glfwCreateWindow(800, 600, "Headless Rendering", nullptr, nullptr);
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
    int width = 512, height = 288;
    glViewport(0, 0, width, height);

    // Create framebuffer and texture
    GLuint framebuffer, textureColorBuffer;
    framebuffer = createFramebuffer(width, height, textureColorBuffer);

    // Load your shader program (pseudo-code, replace with actual shader loading)
    GLuint shaderProgram = loadShader("vertex_shader.glsl", "bloom_shader.glsl");

    // Load the input image as a texture
    GLuint inputTexture;
    glGenTextures(1, &inputTexture);
    glBindTexture(GL_TEXTURE_2D, inputTexture);

    int imgWidth, imgHeight, imgChannels;
    unsigned char* imageData = stbi_load(params.inputpath.c_str(), &imgWidth, &imgHeight, &imgChannels, 0);
    if (!imageData) {
        std::cerr << "Failed to load input image!" << std::endl;
        glfwTerminate();
        return -1;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imgWidth, imgHeight, 0, imgChannels == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, imageData);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(imageData);

    // Render to the framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glViewport(0, 0, width, height);

    // Clear the framebuffer and render with the bloom shader
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);




    glUseProgram(shaderProgram);
    glBindTexture(GL_TEXTURE_2D, inputTexture);


    glUniform1f(glGetUniformLocation(shaderProgram, "bloom_spread"), 10.0f);
    glUniform1f(glGetUniformLocation(shaderProgram, "bloom_intensity"), 6.0f);

    // Full-screen rendering (no quad required; use the default OpenGL pipeline or a full-screen shader)
    glDrawArrays(GL_TRIANGLES, 0, 3); // Use a simple triangle covering the screen if your shader can handle it

    // Save the framebuffer content to an image
    saveFramebufferToImage(framebuffer, "e:\\output.png", width, height);

    // Cleanup
    glDeleteTextures(1, &inputTexture);
    glDeleteTextures(1, &textureColorBuffer);
    glDeleteFramebuffers(1, &framebuffer);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

GLuint loadShader(const char* vertexPath, const char* fragmentPath)
{
    // vertex shader
    std::string vertexCode = R"(
        #version 330 core
        out vec2 TexCoord;

        void main() {
            vec2 pos[3] = vec2[3](
                vec2(-1.0, -1.0), // Bottom-left
                vec2(3.0, -1.0),  // Bottom-right (overshoots for full coverage)
                vec2(-1.0,  3.0)  // Top-left (overshoots for full coverage)
            );
            TexCoord = (pos[gl_VertexID] + 1.0) * 0.5; // Map [-1,1] to [0,1]
            gl_Position = vec4(pos[gl_VertexID], 0.0, 1.0);
        }
    )";


    // pixel shader
    std::string fragmentCode2 = R"(
    #version 330 core

    in vec2 TexCoord;
    out vec4 FragColor;

    uniform sampler2D texture1;
    uniform float bloom_spread = 1.0;
    uniform float bloom_intensity = 2.0;

    void main() {
        ivec2 size = textureSize(texture1, 0);
        ivec2 uv = ivec2(TexCoord * vec2(size));

        vec4 sum = vec4(0.0);
        int offsets[9] = int[9](-4, -3, -2, -1, 0, 1, 2, 3, 4);
        float pixelSizeY = bloom_spread;

        // Vertical blur pass
        for (int n = 0; n < 9; ++n) {
            int offsetY = int(float(offsets[n]) * pixelSizeY);
            vec4 h_sum = vec4(0.0);

            for (int m = 0; m < 9; ++m) {
                int offsetX = int(float(offsets[m]) * bloom_spread);
                h_sum += texelFetch(texture1, uv + ivec2(offsetX, offsetY), 0);
            }

            sum += h_sum / 9.0;
        }

        // Add bloom to the base texture
        FragColor = texture(texture1, TexCoord) + (sum / 81.0) * bloom_intensity;
    }
    )";

    // vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const char* vertexSource = vertexCode.c_str();
    glShaderSource(vertexShader, 1, &vertexSource, nullptr);
    glCompileShader(vertexShader);
    checkCompileErrors(vertexShader, "VERTEX");

    // pixel shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const char* fragmentSource = fragmentCode2.c_str();
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
