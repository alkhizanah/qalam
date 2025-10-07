#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D Texture;

uniform vec4 Color;

void main() {
    float Alpha = texture(Texture, TexCoord).r;

    if (Alpha < 0.1) {
        discard;
    }

    FragColor = vec4(Color * Alpha);
}
