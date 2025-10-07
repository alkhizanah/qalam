#version 330 core

// vec2 Position, vec2 TexCoord, combinded into one vec4
layout (location = 0) in vec4 Vertex;

out vec2 TexCoord;

void main() {
    gl_Position = vec4(Vertex.xy, 0.0, 1.0);
    TexCoord = Vertex.zw;
}
