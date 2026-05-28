#version 330 core
out vec4 FragColor;

uniform vec3 colorLuz;
uniform float brillo;

void main() {
    FragColor = vec4(colorLuz * brillo, 1.0);
}