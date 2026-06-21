#version 330 core
in vec2 TexCoord;
uniform sampler2D textura;
uniform bool usarTextura;

out vec4 FragColor;

void main() {
    if (usarTextura) {
        FragColor = texture(textura, TexCoord);
    } else {
        FragColor = vec4(0.0, 0.0, 0.0, 1.0);
    }
}