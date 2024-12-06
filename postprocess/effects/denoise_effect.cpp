#include "denoise_effect.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

GLuint denoise_effect::apply(const std::string& inputPath, const std::string& outputPath, int width, int height, pmap params)
{
    std::cout << "[INFO] Applying denoise fx" << std::endl;

    float radius = 100.0f;
    
    // Create framebuffer and texture
    GLuint textureColorBuffer;
    GLuint framebuffer = createFramebuffer(width, height, textureColorBuffer);

    // Load your shader program (pseudo-code, replace with actual shader loading)
    GLuint shaderProgram = loadShader(shaders::screen_vert_shader, shaders::denoise_frag_shader);



    // Render to the framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glViewport(0, 0, width, height);

    // Clear the framebuffer and render with the bloom shader
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);




    glUseProgram(shaderProgram);
    


    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_inputTexture);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);

    glUniform1i(glGetUniformLocation(shaderProgram, "width"), width);
    glUniform1i(glGetUniformLocation(shaderProgram, "height"), height);

    glUniform1f(glGetUniformLocation(shaderProgram, "sigma"), 5.0f); // sigma  >  0 - sigma Standard Deviation
    glUniform1f(glGetUniformLocation(shaderProgram, "kSigma"), 3.0f); // kSigma >= 0 - sigma coefficient, kSigma * sigma  -->  radius of the circular kernel
    glUniform1f(glGetUniformLocation(shaderProgram, "threshold"), 0.100f); // threshold   - edge sharpening threshold 



    // Full-screen rendering (no quad required; use the default OpenGL pipeline or a full-screen shader)
    glDrawArrays(GL_TRIANGLES, 0, 3); // Use a simple triangle covering the screen if your shader can handle it


    // Cleanup
    //glDeleteTextures(1, &textureColorBuffer);// probable memory leak to fix

    std::cout << "[INFO] Denoise fx applied successfully" << std::endl;

    return framebuffer;
}