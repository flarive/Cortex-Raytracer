#version 330 core
#define pow2(x)(x*x)
#define PI 3.14159265
uniform sampler2D screenTexture;
uniform bool horizontal;
uniform float radius;

out vec4 FragColor;
float gaussian(float x) {
    float sigma2 = 2. * pow2(radius / 3.);
    return(1. / (sqrt(PI * sigma2))) * exp(-pow2(x) / sigma2);
}
void main() {
    vec2 resolution = vec2(600, 600);
    vec2 uv = vec2(gl_FragCoord.xy / resolution);
    vec4 color = vec4(0.);
    float weight = gaussian(0);
    color += texture2D(screenTexture, uv) * weight;
    float accum = weight;
    if (horizontal) {
        for (int i = 1; i < radius + 1; i++) {
            vec2 off = vec2(i, 0) / resolution;
            weight = gaussian(i);
            color += texture2D(screenTexture, uv + off) * weight;

            color += texture2D(screenTexture, uv - off) * weight;
            accum += weight * 2;
        }
    }
    else {
        for (int i = 1; i < radius + 1; i++) {
            vec2 off = vec2(0, i) / resolution;
            weight = gaussian(i);
            color += texture2D(screenTexture, uv + off) * weight;

            color += texture2D(screenTexture, uv - off) * weight;
            accum += weight * 2;
        }
    }
    FragColor = vec4((color / accum).xyz, 1.);
}