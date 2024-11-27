#include "csb_effect.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

GLuint csb_effect::apply(const std::string& inputPath, const std::string& outputPath, int width, int height, float radius)
{
    std::cout << "[INFO] Applying ContrastSaturationBrightness fx" << std::endl;
    
    // Create framebuffer and texture
    GLuint textureColorBuffer;
    GLuint framebuffer = createFramebuffer(width, height, textureColorBuffer);

    // Load your shader program (pseudo-code, replace with actual shader loading)
    GLuint shaderProgram = loadShader(shaders::screen_vert_shader, shaders::csb_frag_shader);



    // Render to the framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glViewport(0, 0, width, height);

    // Clear the framebuffer and render with the bloom shader
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shaderProgram);
    

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_inputTexture);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);

    // 1.0f is normal value (no changes)
    glUniform1f(glGetUniformLocation(shaderProgram, "contrast"), 1.0f);
    glUniform1f(glGetUniformLocation(shaderProgram, "saturation"), 1.0f);
    glUniform1f(glGetUniformLocation(shaderProgram, "brightness"), 1.0f);
    



    // Full-screen rendering (no quad required; use the default OpenGL pipeline or a full-screen shader)
    glDrawArrays(GL_TRIANGLES, 0, 3); // Use a simple triangle covering the screen if your shader can handle it


    // Cleanup
    //glDeleteTextures(1, &textureColorBuffer);// probable memory leak to fix

    std::cout << "[INFO] ContrastSaturationBrightness fx applied successfully" << std::endl;

    return framebuffer;
}