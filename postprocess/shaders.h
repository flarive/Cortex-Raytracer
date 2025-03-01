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
    /// https://www.shadertoy.com/view/7d2SDD
    /// FAST APPROXIMATION OF https://www.shadertoy.com/view/3dd3Wr
    /// </summary>
    const std::string denoise_frag_shader = R"(
        #version 330 core

        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        //  Copyright (c) 2018-2019 Michele Morrone
        //  All rights reserved.
        //
        //  https://michelemorrone.eu - https://BrutPitt.com
        //
        //  me@michelemorrone.eu - brutpitt@gmail.com
        //  twitter: @BrutPitt - github: BrutPitt
        //  
        //  https://github.com/BrutPitt/glslSmartDeNoise/
        //
        //  This software is distributed under the terms of the BSD 2-Clause license
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

        #define INV_SQRT_OF_2PI 0.39894228040143267793994605993439  // 1.0/SQRT_OF_2PI
        #define INV_PI 0.31830988618379067153776752674503

        //  smartDeNoise - parameters
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        //
        //  sampler2D tex     - sampler image / texture
        //  vec2 uv           - actual fragment coord
        //  float sigma  >  0 - sigma Standard Deviation
        //  float kSigma >= 0 - sigma coefficient 
        //  kSigma * sigma  -->  radius of the circular kernel
        //  float threshold   - edge sharpening threshold 



        in vec2 TexCoord;
        out vec4 FragColor;
        

        uniform sampler2D texture1;


        uniform int width;
        uniform int height;


        
        uniform float sigma = 1.0;
        uniform float kSigma = 0.0;
        uniform float threshold = 1.0;


        vec4 smartDeNoise(sampler2D tex, vec2 uv)
        {
            float radius = round(kSigma*sigma);
            float radQ = radius * radius;
    
            float invSigmaQx2 = .5 / (sigma * sigma);      // 1.0 / (sigma^2 * 2.0)
            float invSigmaQx2PI = INV_PI * invSigmaQx2;    // 1.0 / (sqrt(PI) * sigma)
    
            float invThresholdSqx2 = .5 / (threshold * threshold);     // 1.0 / (sigma^2 * 2.0)
            float invThresholdSqrt2PI = INV_SQRT_OF_2PI / threshold;   // 1.0 / (sqrt(2*PI) * sigma)
    
            vec4 centrPx = texture(texture1,uv);
    
            float zBuff = 0.0;
            vec4 aBuff = vec4(0.0);
            vec2 size = vec2(textureSize(texture1, 0));
    
            for(float x=-radius; x <= radius; x++) {
                float pt = sqrt(radQ-x*x);  // pt = yRadius: have circular trend
                for(float y=-pt; y <= pt; y++) {
                    vec2 d = vec2(x,y);

                    float blurFactor = exp( -dot(d , d) * invSigmaQx2 ) * invSigmaQx2PI; 
            
                    vec4 walkPx =  texture(texture1,uv+d/size);

                    vec4 dC = walkPx-centrPx;
                    float deltaFactor = exp( -dot(dC, dC) * invThresholdSqx2) * invThresholdSqrt2PI * blurFactor;
                                 
                    zBuff += deltaFactor;
                    aBuff += deltaFactor*walkPx;
                }
            }
            return aBuff/zBuff;
        }

        void main()
        {
            // Normalized pixel coordinates (from 0 to 1)
            vec2 resolution = vec2(width, height);
            vec2 uv = vec2(gl_FragCoord.xy / resolution);       

            FragColor = smartDeNoise(texture1, uv);
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

        void main()
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


    // <summary>
    /// GLSL 330
    /// https://prideout.net/blog/old/blog/index.html@p=22.html#toon
    /// </summary>
    const std::string toon_frag_shader = R"(
        #version 330 core

        out vec4 FragColor;
        in vec2 TexCoord;

        uniform sampler2D texture1;
        uniform float textureSizeX;
        uniform float textureSizeY;
        uniform float normalEdgeThreshold;
        uniform float qLevel;
        uniform int bSpecular;
        uniform vec4 ambient;
        uniform vec4 diffuse;
        uniform vec4 specular;
        uniform float shinyness;
                                         
        varying vec3 v;
        varying vec3 N;

        vec3 getNormal(vec2 st){
            vec2 texcoord = clamp(st, 0.001, 0.999);
            return texture2D(texture1, texcoord).rgb;
        }

        void main(void){
            float dxtex = 1.0 / textureSizeX;
            float dytex = 1.0 / textureSizeY;

            vec2 st = TexCoord.st;
            // access center pixel and 4 surrounded pixel
            vec3 center = getNormal(st).rgb;
            vec3 left = getNormal(st + vec2(dxtex, 0.0)).rgb;
            vec3 right = getNormal(st + vec2(-dxtex, 0.0)).rgb;
            vec3 up = getNormal(st + vec2(0.0, -dytex)).rgb;
            vec3 down = getNormal(st + vec2(0.0, dytex)).rgb;

            // discrete Laplace operator
            vec3 laplace = abs(-4.0*center + left + right + up + down);
            // if one rgb-component of convolution result is over threshold => edge
            vec4 line = texture2D(texture1, st);
            if(laplace.r > normalEdgeThreshold
            || laplace.g > normalEdgeThreshold
            || laplace.b > normalEdgeThreshold){
                line = vec4(0.0, 0.0, 0.0, 1.0); // => color the pixel green
            } else {
                line = vec4(1.0, 1.0, 1.0, 1.0); // black
            }

            //end Line;

            //start Phong

            vec3 lightPosition = vec3(100.0, 100.0, 50.0);
            //vec3 lightPosition = gl_LightSource[0].position.xyz;

            vec3 L = normalize(lightPosition - v);
            vec3 E = normalize(-v);
            vec3 R = normalize(-reflect(L,N));

            // ambient term
            vec4 Iamb = ambient;

            // diffuse term
            vec4 Idiff = texture2D( texture1, TexCoord.st) * diffuse;
            //vec4 Idiff = vec4(1.0, 1.0, 1.0, 1.0) * diffuse;
            Idiff *= max(dot(N,L), 0.0);
            Idiff = clamp(Idiff, 0.0, 1.0);

            // specular term
            vec4 Ispec = specular;
            Ispec *= pow(max(dot(R,E),0.0), shinyness);
            Ispec = clamp(Ispec, 0.0, 1.0); 
                
            vec4 color = Iamb + Idiff;
            if ( bSpecular == 1 ) color += Ispec;
            // store previous alpha value
            float alpha = color.a;
            // quantize process: multiply by factor, round and divde by factor
            color = floor(0.5 + (qLevel * color)) / qLevel;
            // set fragment/pixel color
            color.a = alpha;

            FragColor = color * line;
        }
    )";


    // <summary>
    /// GLSL 330
    /// https://prideout.net/blog/old/blog/index.html@p=22.html#toon
    /// </summary>
    const std::string toon_vert_shader = R"(
        #version 330 core

        layout(location = 0) in vec3 aPosition;  // Vertex position
        layout(location = 1) in vec3 aNormal;    // Vertex normal
        layout(location = 2) in vec2 aTexCoord;  // Texture coordinates (if used)

        out vec3 v;            // Output: View-space position
        out vec3 N;            // Output: Normal in view space
        out vec2 TexCoord;     // Output: Texture coordinates

        uniform mat4 model;      // Model matrix
        uniform mat4 view;       // View matrix
        uniform mat4 projection; // Projection matrix

        void main()
        {
            // Calculate the view-space position and normal
            vec4 worldPosition = model * vec4(aPosition, 1.0);
            v = vec3(view * worldPosition);
            N = normalize(mat3(transpose(inverse(view * model))) * aNormal);

            // Pass through texture coordinates
            //TexCoord = aTexCoord;

            //// Final position
            //gl_Position = projection * view * worldPosition;

            vec2 pos[3] = vec2[3](
                vec2(-1.0, -1.0), // Bottom-left
                vec2(3.0, -1.0),  // Bottom-right (overshoots for full coverage)
                vec2(-1.0,  3.0)  // Top-left (overshoots for full coverage)
            );

            TexCoord = (pos[gl_VertexID] + 1.0) * 0.5; // Map [-1,1] to [0,1]
            gl_Position = vec4(pos[gl_VertexID], 0.0, 1.0);
        }
    )";
}