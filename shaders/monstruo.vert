#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in ivec4 aBoneIDs;
layout (location = 4) in vec4 aWeights;

const int MAX_BONES = 100;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 boneMatrices[MAX_BONES];

out vec3 FragPos;
out vec3 Normal;

void main() {
    vec4 pos = vec4(0.0f);
    vec4 normal = vec4(0.0f);
    float totalWeight = 0.0f;

    // ==========================================
    // SKINNING
    // ==========================================
    for (int i = 0; i < 4; i++) {
        if (aWeights[i] > 0.0f) {
            int boneID = aBoneIDs[i];
            if (boneID >= 0 && boneID < MAX_BONES) {
                mat4 boneMat = boneMatrices[boneID];
                pos += boneMat * vec4(aPos, 1.0f) * aWeights[i];
                normal += boneMat * vec4(aNormal, 0.0f) * aWeights[i];
                totalWeight += aWeights[i];
            }
        }
    }

    // Si no hay pesos, usar posición original
    if (totalWeight == 0.0f) {
        pos = vec4(aPos, 1.0f);
        normal = vec4(aNormal, 0.0f);
    }

    FragPos = vec3(model * pos);
    Normal = mat3(transpose(inverse(model))) * vec3(normal);

    gl_Position = projection * view * model * pos;
}