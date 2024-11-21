#version 330 core

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D texture1;
uniform float threshold;

void main() {
    vec3 color = texture(texture1, TexCoord).rgb;
    float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722)); // Luminance
    FragColor = brightness > threshold ? vec4(color, 1.0) : vec4(0.0);
}