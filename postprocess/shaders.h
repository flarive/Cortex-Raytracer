#pragma once

#include <string>

namespace shaders
{
    /// <summary>
    /// GLSL 330
    /// </summary>
    const std::string screen_vert_shader = R"(
        #version 330 core
        out vec2 TexCoord;

        void main() {
            vec2 pos[3] = vec2[3](
                vec2(-1.0, -1.0), // Bottom-left
                vec2(3.0, -1.0),  // Bottom-right (overshoots for full coverage)
                vec2(-1.0,  3.0)  // Top-left (overshoots for full coverage)
            );
            TexCoord = (pos[gl_VertexID] + 1.0) * 0.5; // Map [-1,1] to [0,1]
            gl_Position = vec4(pos[gl_VertexID], 0.0, 1.0);
        }
    )";

    // <summary>
    /// GLSL 330
    /// </summary>
    const std::string brightness_frag_shader = R"(
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
    )";

    // <summary>
    /// GLSL 330
    /// </summary>
    const std::string gaussian_blur_frag_shader = R"(
        #version 330 core
        #define pow2(x)(x*x)
        #define PI 3.14159265

        uniform sampler2D texture1;
        uniform bool horizontal;
        uniform float radius;

        uniform int width;
        uniform int height;

        out vec4 FragColor;
        float gaussian(float x) {
            float sigma2 = 2. * pow2(radius / 3.);
            return(1. / (sqrt(PI * sigma2))) * exp(-pow2(x) / sigma2);
        }
        void main() {
            vec2 resolution = vec2(width, height);
            vec2 uv = vec2(gl_FragCoord.xy / resolution);
            vec4 color = vec4(0.);
            float weight = gaussian(0);
            color += texture2D(texture1, uv) * weight;
            float accum = weight;
            if (horizontal) {
                for (int i = 1; i < radius + 1; i++) {
                    vec2 off = vec2(i, 0) / resolution;
                    weight = gaussian(i);
                    color += texture2D(texture1, uv + off) * weight;

                    color += texture2D(texture1, uv - off) * weight;
                    accum += weight * 2;
                }
            }
            else {
                for (int i = 1; i < radius + 1; i++) {
                    vec2 off = vec2(0, i) / resolution;
                    weight = gaussian(i);
                    color += texture2D(texture1, uv + off) * weight;

                    color += texture2D(texture1, uv - off) * weight;
                    accum += weight * 2;
                }
            }
            FragColor = vec4((color / accum).xyz, 1.);
        }
    )";

    // <summary>
    /// GLSL 330
    /// </summary>
    const std::string combine_frag_shader = R"(
        #version 330 core

        in vec2 TexCoord;
        out vec4 FragColor;

        uniform sampler2D texture1;
        uniform sampler2D texture2;

        void main() {
            vec3 sceneColor = texture(texture1, TexCoord).rgb;
            vec3 bloomColor = texture(texture2, TexCoord).rgb;

            FragColor = vec4(sceneColor + bloomColor, 1.0); // Additive blend
        }
    )";



    // <summary>
    /// GLSL 330
    /// </summary>
    const std::string glow_frag_shader = R"(
        #version 330 core
        /**
        ------------ one pass glow shader ------------

            author: Richman Stewart

            applies a gaussian glow horizontally and vertically
            behind the original texture

        ------------------ use ------------------------

            glow_size - defines the spread x and y
            glow_colour - the colour of the glow
            glow_intensity - glow intensity

        **/

        in vec4 v_colour;
        in vec2 TexCoord;
        out vec4 FragColor;

        uniform sampler2D texture1;
        uniform float glow_size = .5;
        uniform vec3 glow_colour = vec3(0, 0, 0);
        uniform float glow_intensity = 1;
        uniform float glow_threshold = .5;

        void main() {
            FragColor = texture(texture1, TexCoord);
            if (FragColor.a <= glow_threshold) {
                ivec2 size = textureSize(texture1, 0);
	
                float uv_x = TexCoord.x * size.x;
                float uv_y = TexCoord.y * size.y;

                float sum = 0.0;
                for (int n = 0; n < 9; ++n) {
                    uv_y = (TexCoord.y * size.y) + (glow_size * float(n - 4.5));
                    float h_sum = 0.0;
                    h_sum += texelFetch(texture1, ivec2(uv_x - (4.0 * glow_size), uv_y), 0).a;
                    h_sum += texelFetch(texture1, ivec2(uv_x - (3.0 * glow_size), uv_y), 0).a;
                    h_sum += texelFetch(texture1, ivec2(uv_x - (2.0 * glow_size), uv_y), 0).a;
                    h_sum += texelFetch(texture1, ivec2(uv_x - glow_size, uv_y), 0).a;
                    h_sum += texelFetch(texture1, ivec2(uv_x, uv_y), 0).a;
                    h_sum += texelFetch(texture1, ivec2(uv_x + glow_size, uv_y), 0).a;
                    h_sum += texelFetch(texture1, ivec2(uv_x + (2.0 * glow_size), uv_y), 0).a;
                    h_sum += texelFetch(texture1, ivec2(uv_x + (3.0 * glow_size), uv_y), 0).a;
                    h_sum += texelFetch(texture1, ivec2(uv_x + (4.0 * glow_size), uv_y), 0).a;
                    sum += h_sum / 9.0;
                }

                FragColor = vec4(glow_colour, (sum / 9.0) * glow_intensity);
            }
        }
    )";
}