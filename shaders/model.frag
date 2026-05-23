#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform vec3 objectColor;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform bool hasTexture;

// Para múltiples texturas
struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_diffuse2;
    sampler2D texture_diffuse3;
    sampler2D texture_specular1;
    sampler2D texture_specular2;
};

uniform Material material;

void main() {
    vec3 color;

    if(hasTexture) {
        // Usar la primera textura difusa
        color = texture(material.texture_diffuse1, TexCoord).rgb;
    } else {
        // Usar color sólido
        color = objectColor;
    }

    // Luz ambiental
    float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * vec3(1.0f);

    // Luz difusa
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * vec3(1.0f);

    // Atenuación
    float distance = length(lightPos - FragPos);
    float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * (distance * distance));

    vec3 result = (ambient + diffuse) * color * attenuation;

    // Brillo mínimo
    result = max(result, color * 0.15);

    FragColor = vec4(result, 1.0);
}