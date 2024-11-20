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