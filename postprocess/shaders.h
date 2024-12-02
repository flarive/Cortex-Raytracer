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

    /// <summary>
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

        uniform sampler2D texture1; // Scene texture
        uniform sampler2D texture2; // Bloom texture
        uniform float intensity; // Intensity of the bloom effect
        uniform float maxBloom; // Maximum allowed brightness for the bloom effect

        void main() {
            vec3 sceneColor = texture(texture1, TexCoord).rgb;
            vec3 bloomColor = texture(texture2, TexCoord).rgb;

            // Clamp the bloom color to the maximum allowed value
            bloomColor = clamp(bloomColor, vec3(0.0), vec3(maxBloom));

            // Blend the bloom color with the scene color using the intensity parameter
            vec3 finalColor = sceneColor + clamp(intensity * bloomColor, vec3(0.0), vec3(maxBloom));

            FragColor = vec4(finalColor, 1.0);
        }
    )";



    // <summary>
    /// GLSL 330
    /// </summary>
    const std::string glow_frag_shader = R"(
        #version 330 core

        in vec2 TexCoord;
        out vec4 FragColor;

        uniform int width;
        uniform int height;

        
        uniform sampler2D texture1;
        

        void main()
        {
          vec2 resolution = vec2(width, height);
          vec2 uv = vec2(gl_FragCoord.xy / resolution);

          
          
          uv -= 0.5; // x: <-0.5, 0.5>, y: <-0.5, 0.5>
          uv.x *= resolution.x/resolution.y; // x: <-0.5, 0.5> * aspect ratio, y: <-0.5, 0.5>

          float d = length(uv) - 0.2; // signed distance function

          //vec3 col = vec3(step(0., -d)); // create white circle with black background
          vec3 col = texture(texture1, TexCoord).rgb;

          float glow = 0.01/d; // create glow and diminish it with distance
          glow = clamp(glow, 0., 1.); // remove artifacts

          col += glow * 5.; // add glow

          FragColor = vec4(col,1.0); // output color
        }
    )";


    // <summary>
    /// GLSL 330
    /// </summary>
    const std::string csb_frag_shader = R"(
        #version 330 core

        uniform sampler2D texture;
        uniform vec2 texOffset;

        in vec4 vertColor;
        in vec2 TexCoord;
        out vec4 FragColor;

        uniform float contrast;
        uniform float saturation;
        uniform float brightness;

        // For all settings: 1.0 = 100% 0.5=50% 1.5 = 150%
        vec3 ContrastSaturationBrightness(vec3 color, float brt, float sat, float con)
        {
	        // Increase or decrease theese values to adjust r, g and b color channels seperately
	        const float AvgLumR = 0.5;
	        const float AvgLumG = 0.5;
	        const float AvgLumB = 0.5;
	
	        const vec3 LumCoeff = vec3(0.2125, 0.7154, 0.0721);
	
	        vec3 AvgLumin  = vec3(AvgLumR, AvgLumG, AvgLumB);
	        vec3 brtColor  = color * brt;
	        vec3 intensity = vec3(dot(brtColor, LumCoeff));
	        vec3 satColor  = mix(intensity, brtColor, sat);
	        vec3 conColor  = mix(AvgLumin, satColor, con);
	
	        return conColor;
        }

        void main(void)
        {
	        vec2 coord = TexCoord.st;

            // Get the color of the pixel at our fragment's coordinates
            vec4 pixel = texture2D( texture, coord );
	
	        // Apply filter to current fragment
	        vec3 color = ContrastSaturationBrightness( pixel.rgb, brightness, saturation, contrast );

	        float alpha = 1.0;
	
	        FragColor = vec4( color, alpha );
        }
    )";

    // <summary>
    /// GLSL 330
    /// https://www.shadertoy.com/view/Xst3W7
    /// </summary>
    const std::string floydsteinberg_frag_shader = R"(
        #version 330 core

        out vec4 FragColor;
        in vec2 TexCoord;

        uniform vec2 uResolution;    // Resolution of the viewport
        uniform sampler2D texture1;  // Input texture

        const vec2 gridSize = vec2(3.0, 3.0);
        const int arrSize = int(gridSize.x * gridSize.y);
        float errAcc[arrSize];

        int getIndex(vec2 p) {
            int x = int(p.x);
            int y = int(p.y);
            if (x >= int(gridSize.x) || y >= int(gridSize.y) || x < 0 || y < 0) {
                return -1;
            }
            return x + y * int(gridSize.x);
        }

        float getValue(vec2 p) {
            int index = getIndex(p);
            if (index < 0) return 0.0;
            for (int i = 0; i < arrSize; i++) {
                if (i == index) return errAcc[i];
            }
            return 0.0;
        }

        void accValue(vec2 p, float value) {
            int index = getIndex(p);
            if (index < 0) return;
            for (int i = 0; i < arrSize; i++) {
                if (i == index) {
                    errAcc[i] += value;
                    return;
                }
            }
        }

        void main() {
            // Zero-initialize error accumulator
            for (int i = 0; i < arrSize; ++i) {
                errAcc[i] = 0.0;
            }

            vec2 pix = TexCoord * uResolution;
            vec2 topLeftBound = floor(pix / gridSize) * gridSize;

            float outpColor = 0.0;
            float errCarry = 0.0;

            for (float x = gridSize.x - 1.0; x >= 0.0; --x) {
                for (float y = gridSize.y - 1.0; y >= 0.0; --y) {
                    vec2 absSamplePt = pix - vec2(x, y);
                    vec2 gridPos = absSamplePt - topLeftBound;
                    if (gridPos.x < 0.0 || gridPos.y < 0.0) continue;

                    vec2 uv = absSamplePt / uResolution;
                    vec3 oc = texture(texture1, uv).rgb;
                    float og = (oc.r + oc.g + oc.b) / 3.0;

                    float err = getValue(gridPos);
                    float idealColorWithErr = og + err;
                    outpColor = step(0.0, idealColorWithErr - 0.5);
                    err = idealColorWithErr - outpColor;

                    // Floyd-Steinberg dithering
                    accValue(gridPos + vec2(1, 0), err * (7.0 / 16.0));
                    accValue(gridPos + vec2(-1, 1), err * (3.0 / 16.0));
                    accValue(gridPos + vec2(0, 1), err * (5.0 / 16.0));
                    accValue(gridPos + vec2(1, 1), err * (1.0 / 16.0));
                }
            }

            FragColor = vec4(outpColor, outpColor, outpColor, 1.0); // Output grayscale
        }
    )";
}