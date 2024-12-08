#include "toon_effect.h"

GLuint toon_effect::apply(const std::string& inputPath, const std::string& outputPath, int width, int height, pmap params)
{
    std::cout << "[INFO] Applying toon fx" << std::endl;

    //pvar v_contrast = 0.0f;
    //pvar v_saturation = 0.0f;
    //pvar v_brightness = 0.0f;


    //if (params.count("contrast") > 0)
    //    v_contrast = params.at("contrast");

    //if (params.count("saturation") > 0)
    //    v_saturation = params.at("saturation");

    //if (params.count("brightness") > 0)
    //    v_brightness = params.at("brightness");


    //float contrast = std::get<float>(v_contrast);
    //float saturation = std::get<float>(v_saturation);
    //float brightness = std::get<float>(v_brightness);



    // Create framebuffer and texture
    GLuint textureColorBuffer;
    GLuint framebuffer = createFramebuffer(width, height, textureColorBuffer);

    // Load your shader program (pseudo-code, replace with actual shader loading)
    GLuint shaderProgram = loadShader(shaders::toon_vert_shader, shaders::toon_frag_shader);



    // Render to the framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glViewport(0, 0, width, height);

    // Clear the framebuffer and render with the bloom shader
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shaderProgram);


    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_inputTexture);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);


    glUniform1f(glGetUniformLocation(shaderProgram, "textureSizeX"), width);
    glUniform1f(glGetUniformLocation(shaderProgram, "textureSizeY"), height);



    // Assuming `shaderProgram` is your shader program ID and `myVec3` is the glm::vec3 you want to send
    //glm::vec3 myVec3(1.0f, 2.0f, 3.0f); // Example glm::vec3
    //GLint vec3Location = glGetUniformLocation(shaderProgram, "LightPosition");
    //glUniform3fv(vec3Location, 1, &myVec3[0]);


    //glm::vec3 myVec3b(1.0f, 0.0f, 0.0f); // Example glm::vec3
    //GLint vec3Locationb = glGetUniformLocation(shaderProgram, "AmbientMaterial");
    //glUniform3fv(vec3Locationb, 1, &myVec3b[0]);

    //
    //glm::vec3 myVec3c(1.0f, 0.0f, 0.0f); // Example glm::vec3
    //GLint vec3Locationc = glGetUniformLocation(shaderProgram, "SpecularMaterial");
    //glUniform3fv(vec3Locationc, 1, &myVec3c[0]);

    float edgeThreshold = 0.2;
    glUniform1f(glGetUniformLocation(shaderProgram, "normalEdgeThreshold"), 2.0f);

    float level = 1.0;
    glUniform1f(glGetUniformLocation(shaderProgram, "qLevel"), level);

    bool isSpecular = false;
    glUniform1i(glGetUniformLocation(shaderProgram, "bSpecular"), isSpecular);


    glm::vec4 myVec4(0.1, 0.1, 0.1, 1.0); // Example glm::vec3
    GLint vec4Location = glGetUniformLocation(shaderProgram, "ambient");
    glUniform4fv(vec4Location, 1, &myVec4[0]);

    glm::vec4 myVec4b(0.9, 0.9, 0.9, 1.0); // Example glm::vec3
    GLint vec4Locationb = glGetUniformLocation(shaderProgram, "diffuse");
    glUniform4fv(vec4Locationb, 1, &myVec4b[0]);

    glm::vec4 myVec4c(1, 1, 1, 1); // Example glm::vec3
    GLint vec4Locationc = glGetUniformLocation(shaderProgram, "specular");
    glUniform4fv(vec4Locationc, 1, &myVec4c[0]);


    float shinyness = 100;
    glUniform1f(glGetUniformLocation(shaderProgram, "shinyness"), shinyness);


    // Full-screen rendering (no quad required; use the default OpenGL pipeline or a full-screen shader)
    glDrawArrays(GL_TRIANGLES, 0, 3); // Use a simple triangle covering the screen if your shader can handle it


    // Cleanup
    //glDeleteTextures(1, &textureColorBuffer);// probable memory leak to fix

    std::cout << "[INFO] Toon fx applied successfully" << std::endl;

    return framebuffer;
}