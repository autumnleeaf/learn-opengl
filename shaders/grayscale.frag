#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;

void main() {
    FragColor = texture(screenTexture, TexCoords);
    float average = (FragColor.r * 0.2126) + (FragColor.g * 0.7152) + (FragColor.b * 0.0722);
    FragColor = vec4(average, average, average, 1.0);
}
