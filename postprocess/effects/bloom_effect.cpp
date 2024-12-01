#include "bloom_effect.h"

GLuint bloom_effect::apply(const std::string& inputPath, const std::string& outputPath, int width, int height, pmap params)
{
    std::cout << "[INFO] Applying bloom fx" << std::endl;


    pvar v_threshold = 0.5f;
    pvar v_radius = 10.0f;
    pvar v_intensity = 1.0f;
    pvar v_maxbloom = 1.0f;
    


    if (params.count("threshold") > 0)
        v_threshold = params.at("threshold");

    if (params.count("radius") > 0)
        v_radius = params.at("radius");

    if (params.count("intensity") > 0)
        v_intensity = params.at("intensity");

    if (params.count("maxbloom") > 0)
        v_maxbloom = params.at("maxbloom");


    float threshold = std::get<float>(v_threshold);
    float radius = std::get<float>(v_radius);
    float intensity = std::get<float>(v_intensity);
    float maxBloom = std::get<float>(v_maxbloom);

    
    // Textures
    GLuint brightTexture, horizontalTexture, verticalTexture, combineTexture;

    // Framebuffers
    GLuint brightFramebuffer = createFramebuffer(width, height, brightTexture);
    GLuint horizontalFramebuffer = createFramebuffer(width, height, horizontalTexture);
    GLuint verticalFramebuffer = createFramebuffer(width, height, verticalTexture);
    GLuint combineFramebuffer = createFramebuffer(width, height, combineTexture);

    // Load shaders
    GLuint brightnessShader = loadShader(shaders::screen_vert_shader, shaders::brightness_frag_shader);
    GLuint blurShader = loadShader(shaders::screen_vert_shader, shaders::gaussian_blur_frag_shader);
    GLuint combineShader = loadShader(shaders::screen_vert_shader, shaders::combine_frag_shader);




    // Brightness extraction shader pass
    glBindFramebuffer(GL_FRAMEBUFFER, brightFramebuffer);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(brightnessShader);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_inputTexture);
    glUniform1i(glGetUniformLocation(brightnessShader, "texture1"), 0);
    glUniform1f(glGetUniformLocation(brightnessShader, "threshold"), threshold); // Adjust threshold
    glDrawArrays(GL_TRIANGLES, 0, 3);




    // Horizontal blur shader pass
    glBindFramebuffer(GL_FRAMEBUFFER, horizontalFramebuffer);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(blurShader);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, brightTexture);
    glUniform1i(glGetUniformLocation(blurShader, "texture1"), 0);
    glUniform1i(glGetUniformLocation(blurShader, "horizontal"), GL_TRUE);
    glUniform1i(glGetUniformLocation(blurShader, "width"), width);
    glUniform1i(glGetUniformLocation(blurShader, "height"), height);
    glUniform1f(glGetUniformLocation(blurShader, "radius"), radius);
    glDrawArrays(GL_TRIANGLES, 0, 3);



    // Vertical blur shader pass
    glBindFramebuffer(GL_FRAMEBUFFER, verticalFramebuffer);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(blurShader);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, horizontalTexture);
    glUniform1i(glGetUniformLocation(blurShader, "texture1"), 0);
    glUniform1i(glGetUniformLocation(blurShader, "horizontal"), GL_FALSE);
    glUniform1i(glGetUniformLocation(blurShader, "width"), width);
    glUniform1i(glGetUniformLocation(blurShader, "height"), height);
    glUniform1f(glGetUniformLocation(blurShader, "radius"), radius);
    glDrawArrays(GL_TRIANGLES, 0, 3);




    // Combine textures shader pass
    glBindFramebuffer(GL_FRAMEBUFFER, combineFramebuffer); // Render to default framebuffer
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(combineShader);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_inputTexture);
    glUniform1i(glGetUniformLocation(combineShader, "texture1"), 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, verticalTexture);
    glUniform1i(glGetUniformLocation(combineShader, "texture2"), 1);
    glUniform1f(glGetUniformLocation(combineShader, "intensity"), intensity);
    glUniform1f(glGetUniformLocation(combineShader, "maxBloom"), maxBloom);

    

    


    // Full-screen rendering (no quad required; use the default OpenGL pipeline or a full-screen shader)
    glDrawArrays(GL_TRIANGLES, 0, 3); // Use a simple triangle covering the screen if your shader can handle it


    // Cleanup
    glDeleteTextures(1, &brightTexture);
    glDeleteTextures(1, &horizontalTexture);
    glDeleteTextures(1, &verticalTexture);
    //glDeleteTextures(1, &combineTexture); // probable memory leak to fix

    glDeleteFramebuffers(1, &brightFramebuffer);
    glDeleteFramebuffers(1, &horizontalFramebuffer);
    glDeleteFramebuffers(1, &verticalFramebuffer);


    std::cout << "[INFO] Bloom fx applied successfully" << std::endl;

    return combineFramebuffer;
}
