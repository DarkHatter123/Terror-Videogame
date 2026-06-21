#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 color;

uniform float scale;

void main()
{
    vec3 pos = aPos;
    pos.x = pos.x * scale;
    pos.y = pos.y * scale;
    gl_Position = vec4(pos, 1.0);
    color = aColor;
} 