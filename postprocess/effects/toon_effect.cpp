#include "toon_effect.h"

GLuint toon_effect::apply(const std::string& inputPath, const std::string& outputPath, int width, int height, pmap params)
{
    std::cout << "[INFO] Applying toon fx" << std::endl;

    pvar v_contrast = 0.0f;
    pvar v_saturation = 0.0f;
    pvar v_brightness = 0.0f;


    if (params.count("contrast") > 0)
        v_contrast = params.at("contrast");

    if (params.count("saturation") > 0)
        v_saturation = params.at("saturation");

    if (params.count("brightness") > 0)
        v_brightness = params.at("brightness");


    float contrast = std::get<float>(v_contrast);
    float saturation = std::get<float>(v_saturation);
    float brightness = std::get<float>(v_brightness);



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


    glUniform1i(glGetUniformLocation(shaderProgram, "width"), width);
    glUniform1i(glGetUniformLocation(shaderProgram, "height"), height);


    //glUniform3f(rc.ToonUniforms.DiffuseMaterial, 0, 0.75, 0.75);
    //glUniform3f(rc.ToonUniforms.AmbientMaterial, 0.04f, 0.04f, 0.04f);
    //glUniform3f(rc.ToonUniforms.SpecularMaterial, 0.5, 0.5, 0.5);
    //glUniform1f(rc.ToonUniforms.Shininess, 50);

    //vec4 lightPosition(0.25, 0.25, 1, 0);
    //glUniform3fv(rc.ToonUniforms.LightPosition, 1, lightPosition.Pointer());

    //glUniformMatrix4fv(rc.ToonUniforms.Projection, 1, 0, rc.Projection.Pointer());
    //glUniformMatrix4fv(rc.ToonUniforms.Modelview, 1, 0, rc.Modelview.Pointer());
    //glUniformMatrix3fv(rc.ToonUniforms.NormalMatrix, 1, 0, rc.NormalMatrix.Pointer());



    // Full-screen rendering (no quad required; use the default OpenGL pipeline or a full-screen shader)
    glDrawArrays(GL_TRIANGLES, 0, 3); // Use a simple triangle covering the screen if your shader can handle it


    // Cleanup
    //glDeleteTextures(1, &textureColorBuffer);// probable memory leak to fix

    std::cout << "[INFO] Toon fx applied successfully" << std::endl;

    return framebuffer;
}