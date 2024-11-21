#version 330 core

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D scene;
uniform sampler2D bloom;

void main() {
    //vec3 sceneColor = texture(scene, TexCoord).rgb;
    vec3 sceneColor = vec3(1.0, 0.0, 0.0);
    vec3 bloomColor = texture(bloom, TexCoord).rgb;

    FragColor = vec4(sceneColor + bloomColor, 1.0); // Additive blend
}
