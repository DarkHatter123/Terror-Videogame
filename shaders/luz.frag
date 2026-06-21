#version 330 core
out vec4 FragColor;

uniform vec3 colorLuz;
uniform float brillo;

void main() {
    FragColor = vec4(colorLuz, 1.0) * brillo;
}