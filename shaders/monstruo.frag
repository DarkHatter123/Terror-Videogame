#version 330 core
out vec4 FragColor;

uniform vec3 objectColor;
uniform vec3 viewPos;

in vec3 FragPos;
in vec3 Normal;

void main() {
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(vec3(5.0f, 10.0f, 5.0f) - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = vec3(1.0f, 1.0f, 1.0f) * diff;

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), 32.0);
    vec3 specular = vec3(1.0f, 1.0f, 1.0f) * spec * 0.5f;

    vec3 ambient = vec3(0.1f, 0.1f, 0.1f);
    vec3 result = (ambient + diffuse + specular) * objectColor;

    FragColor = vec4(result, 1.0);
}