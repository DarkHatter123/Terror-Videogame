#version 330 core
out vec4 FragColor;

in vec3 TexCoords;
uniform vec3 color;

void main() {
    // Sin textura, solo color sólido oscuro con degradado
    float intensity = (TexCoords.y + 1.0) * 0.3 + 0.2;
    FragColor = vec4(color * intensity, 1.0);
}