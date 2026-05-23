#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

// Estructura común para focos (tanto lámparas fijas como linterna)
struct SpotLight {
    vec3 position;
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constant;
    float linear;
    float quadratic;
    float cutOff;
    float outerCutOff;
};

#define NR_SPOT_LIGHTS 16
uniform SpotLight spotLights[NR_SPOT_LIGHTS];
uniform int numSpotLights;
uniform SpotLight flashlight;
uniform bool flashlightOn;
uniform vec3 objectColor;
uniform vec3 viewPos;
uniform sampler2D textura;
uniform bool usarTextura;

void main() {
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 colorBase;
    if (usarTextura) {
        colorBase = texture(textura, TexCoord).rgb;
    } else {
        colorBase = objectColor;
    }

    vec3 result = vec3(0.0);

    // ========== LUCES FIJAS (lámparas del techo) ==========
    for (int i = 0; i < numSpotLights; i++) {
        float distance = length(spotLights[i].position - FragPos);
        float attenuation = 1.0 / (spotLights[i].constant +
        spotLights[i].linear * distance +
        spotLights[i].quadratic * distance * distance);

        vec3 ambient = spotLights[i].ambient * colorBase;
        vec3 lightDir = normalize(spotLights[i].position - FragPos);
        float theta = dot(lightDir, normalize(-spotLights[i].direction));
        float epsilon = spotLights[i].cutOff - spotLights[i].outerCutOff;
        float intensity = clamp((theta - spotLights[i].outerCutOff) / epsilon, 0.0, 1.0);

        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = spotLights[i].diffuse * diff * colorBase * intensity;

        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16.0);
        vec3 specular = spotLights[i].specular * spec * intensity;

        result += (ambient + diffuse + specular) * attenuation;
    }

    // ========== LINTERNA DEL JUGADOR ==========
    if (flashlightOn) {
        float distance = length(flashlight.position - FragPos);
        float attenuation = 1.0 / (flashlight.constant +
        flashlight.linear * distance +
        flashlight.quadratic * distance * distance);

        vec3 ambient = flashlight.ambient * colorBase;
        vec3 lightDir = normalize(flashlight.position - FragPos);
        float theta = dot(lightDir, normalize(-flashlight.direction));
        float epsilon = flashlight.cutOff - flashlight.outerCutOff;
        float intensity = clamp((theta - flashlight.outerCutOff) / epsilon, 0.0, 1.0);

        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = flashlight.diffuse * diff * colorBase * intensity;

        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16.0);
        vec3 specular = flashlight.specular * spec * intensity;

        result += (ambient + diffuse + specular) * attenuation;
    }

    // Mínimo de luz para evitar negro total (ajústalo después)
    result = max(result, colorBase * 0.02);

    FragColor = vec4(result, 1.0);
}