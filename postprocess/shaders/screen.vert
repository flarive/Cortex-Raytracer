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