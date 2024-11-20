#version 330 core

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D texture1;       // Input texture
uniform bool horizontal;          // Direction of blur: horizontal or vertical
uniform int blurRadius;           // Number of samples in each direction
uniform float blurScale;          // Scaling factor for sample distance
uniform float weights[256];       // Precomputed weights (e.g., Gaussian)

void main() {
    vec2 texelSize = 1.0 / textureSize(texture1, 0); // Size of one texel
    vec2 direction = horizontal ? vec2(texelSize.x, 0.0) : vec2(0.0, texelSize.y);

    vec4 color = vec4(0.0); // Accumulator for the blur color
    float totalWeight = 0.0; // Accumulator for normalization

    // Sample in both positive and negative directions
    for (int i = -blurRadius; i <= blurRadius; ++i) {
        float weight = weights[abs(i)];
        vec2 offset = direction * float(i) * blurScale;

        color += texture(texture1, TexCoord + offset) * weight;
        totalWeight += weight;
    }

    // Normalize the final color
    FragColor = color / totalWeight;
}
