<<<<<<< HEAD

=======
>>>>>>> 5ef2c6448ea51885101b988eaf507094d924b7a3
#include "Model.h"
#include "src/SOIL2/SOIL2.h"
#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
<<<<<<< HEAD
#include <glm/gtc/type_ptr.hpp>
=======
>>>>>>> 5ef2c6448ea51885101b988eaf507094d924b7a3

Model::Model(const char* path) {
    position = glm::vec3(0.0f);
    rotation = glm::vec3(0.0f);
    scale = glm::vec3(1.0f);
    boneCount = 0;
    currentAnimIndex = 0;
    animTime = 0.0f;
    animLoop = true;
    boneTransforms.resize(MAX_BONES);
    loadModel(path);
}

void Model::Draw(Shader &shader) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, scale);

    shader.setMat4("model", model);
    shader.setBool("usarTextura", true);
<<<<<<< HEAD

    if (!animations.empty() && !boneTransforms.empty()) {
        shader.setMat4Array("boneMatrices", boneCount, boneTransforms.data());
        shader.setInt("numBones", boneCount);
    } else {
        shader.setInt("numBones", 0);
    }
=======
>>>>>>> 5ef2c6448ea51885101b988eaf507094d924b7a3

    for(unsigned int i = 0; i < meshes.size(); i++)
        meshes[i].Draw(shader.ID);
}

void Model::DrawAnimated(Shader &shader) {
    computeBoneTransforms(animTime);
    Draw(shader);
}

void Model::setAnimation(float time) {
    animTime = time;
}

void Model::updateAnimation(float deltaTime) {
    if (animations.empty()) return;
    const Animation& anim = animations[currentAnimIndex];
    animTime += deltaTime * anim.ticksPerSecond;
    if (animLoop) {
        while (animTime >= anim.duration)
            animTime -= anim.duration;
        while (animTime < 0.0f)
            animTime += anim.duration;
    } else {
        if (animTime > anim.duration) animTime = anim.duration;
    }
}

float Model::getAnimationDuration() const {
    if (animations.empty()) return 0.0f;
    return animations[currentAnimIndex].duration;
}

void Model::loadModel(std::string path) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path,
        aiProcess_Triangulate |
        aiProcess_FlipUVs |
        aiProcess_GenNormals |
        aiProcess_CalcTangentSpace |
        aiProcess_LimitBoneWeights);

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return;
    }
    directory = path.substr(0, path.find_last_of('/'));
    if(directory.empty()) {
        directory = path.substr(0, path.find_last_of('\\'));
    }

    globalInverseTransform = glm::mat4(1.0f);
    if (scene->mRootNode) {
        aiMatrix4x4 m = scene->mRootNode->mTransformation;
        glm::mat4 rootTransform = glm::mat4(
            m.a1, m.b1, m.c1, m.d1,
            m.a2, m.b2, m.c2, m.d2,
            m.a3, m.b3, m.c3, m.d3,
            m.a4, m.b4, m.c4, m.d4
        );
        globalInverseTransform = glm::inverse(rootTransform);
    }

    processNode(scene->mRootNode, scene);
<<<<<<< HEAD
    rootNode = buildNodeTree(scene->mRootNode);
    loadAnimations(scene);
    std::cout << "Modelo cargado: " << path << " (" << meshes.size() << " meshes, " << boneCount << " bones, " << animations.size() << " animations)" << std::endl;
=======
    std::cout << "Modelo cargado: " << path << " (" << meshes.size() << " meshes)" << std::endl;
>>>>>>> 5ef2c6448ea51885101b988eaf507094d924b7a3
}

void Model::processNode(aiNode *node, const aiScene *scene) {
    for(unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }
    for(unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

NodeData Model::buildNodeTree(aiNode* node) {
    NodeData data;
    data.name = node->mName.C_Str();
    aiMatrix4x4 m = node->mTransformation;
    data.transform = glm::mat4(
        m.a1, m.b1, m.c1, m.d1,
        m.a2, m.b2, m.c2, m.d2,
        m.a3, m.b3, m.c3, m.d3,
        m.a4, m.b4, m.c4, m.d4
    );
    for(unsigned int i = 0; i < node->mNumChildren; i++) {
        data.children.push_back(buildNodeTree(node->mChildren[i]));
    }
    return data;
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    for(unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;
        vertex.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        vertex.Normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);

        if(mesh->mTextureCoords[0]) {
            vertex.TexCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        } else {
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);
        }
        vertices.push_back(vertex);
    }

<<<<<<< HEAD
    std::vector<int> boneIds(vertices.size(), -1);
    std::vector<float> boneWeights(vertices.size(), 0.0f);
    extractBoneData(mesh, boneIds, boneWeights);

    for(size_t i = 0; i < vertices.size(); i++) {
        if (boneIds[i] >= 0) {
            vertices[i].boneIds.x = boneIds[i];
            vertices[i].weights.x = boneWeights[i];
        }
    }

=======
>>>>>>> 5ef2c6448ea51885101b988eaf507094d924b7a3
    for(unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for(unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    if(mesh->mMaterialIndex >= 0) {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

        std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    }

    return Mesh(vertices, indices, textures);
}

void Model::extractBoneData(aiMesh* mesh, std::vector<int>& boneIds, std::vector<float>& weights) {
    for(unsigned int i = 0; i < mesh->mNumBones; i++) {
        aiBone* bone = mesh->mBones[i];
        std::string boneName = bone->mName.C_Str();

        int boneId = -1;
        if (boneMap.find(boneName) != boneMap.end()) {
            boneId = boneMap[boneName];
        } else {
            boneId = boneCount;
            boneCount++;
            BoneInfo info;
            info.id = boneId;
            info.name = boneName;
            aiMatrix4x4 m = bone->mOffsetMatrix;
            info.offset = glm::mat4(
                m.a1, m.b1, m.c1, m.d1,
                m.a2, m.b2, m.c2, m.d2,
                m.a3, m.b3, m.c3, m.d3,
                m.a4, m.b4, m.c4, m.d4
            );
            boneInfo.push_back(info);
            boneMap[boneName] = boneId;
        }

        for(unsigned int j = 0; j < bone->mNumWeights; j++) {
            int vertexId = bone->mWeights[j].mVertexId;
            float weight = bone->mWeights[j].mWeight;
            if (vertexId < (int)boneIds.size()) {
                for (int k = 0; k < 4; k++) {
                    if (boneIds[vertexId] < 0) {
                        boneIds[vertexId] = boneId;
                        weights[vertexId] = weight;
                        break;
                    }
                }
            }
        }
    }
}

void Model::loadAnimations(const aiScene* scene) {
    if (!scene->mNumAnimations) return;

    for(unsigned int i = 0; i < scene->mNumAnimations; i++) {
        aiAnimation* anim = scene->mAnimations[i];
        Animation animation;
        animation.name = anim->mName.C_Str();
        animation.duration = (float)anim->mDuration;
        animation.ticksPerSecond = (float)(anim->mTicksPerSecond != 0 ? anim->mTicksPerSecond : 25.0f);

        for(unsigned int j = 0; j < anim->mNumChannels; j++) {
            aiNodeAnim* channel = anim->mChannels[j];
            BoneAnim boneAnim;
            boneAnim.name = channel->mNodeName.C_Str();

            if (boneMap.find(boneAnim.name) != boneMap.end()) {
                boneAnim.boneId = boneMap[boneAnim.name];
            } else {
                boneAnim.boneId = -1;
            }

            for(unsigned int k = 0; k < channel->mNumPositionKeys; k++) {
                BoneKeyPos key;
                key.time = (float)channel->mPositionKeys[k].mTime;
                key.pos = glm::vec3(
                    channel->mPositionKeys[k].mValue.x,
                    channel->mPositionKeys[k].mValue.y,
                    channel->mPositionKeys[k].mValue.z
                );
                boneAnim.positions.push_back(key);
            }

            for(unsigned int k = 0; k < channel->mNumRotationKeys; k++) {
                BoneKeyRot key;
                key.time = (float)channel->mRotationKeys[k].mTime;
                key.rot = glm::quat(
                    channel->mRotationKeys[k].mValue.w,
                    channel->mRotationKeys[k].mValue.x,
                    channel->mRotationKeys[k].mValue.y,
                    channel->mRotationKeys[k].mValue.z
                );
                boneAnim.rotations.push_back(key);
            }

            for(unsigned int k = 0; k < channel->mNumScalingKeys; k++) {
                BoneKeyScale key;
                key.time = (float)channel->mScalingKeys[k].mTime;
                key.scale = glm::vec3(
                    channel->mScalingKeys[k].mValue.x,
                    channel->mScalingKeys[k].mValue.y,
                    channel->mScalingKeys[k].mValue.z
                );
                boneAnim.scales.push_back(key);
            }

            animation.bones.push_back(boneAnim);
        }

        animations.push_back(animation);
        std::cout << "Animacion cargada: " << animation.name
                  << " (duracion: " << animation.duration
                  << " ticks, " << animation.ticksPerSecond << " tps, "
                  << animation.bones.size() << " canales)" << std::endl;
    }
}

void Model::computeBoneTransforms(float time) {
    if (animations.empty()) return;
    readNodeHierarchy(time, rootNode, glm::mat4(1.0f));
}

void Model::readNodeHierarchy(float time, const NodeData& node, const glm::mat4& parentTransform, int depth) {
    const Animation& anim = animations[currentAnimIndex];

    glm::mat4 nodeTransform = node.transform;

    int boneAnimIndex = -1;
    if (!node.name.empty()) {
        boneAnimIndex = findBoneAnimIndex(node.name);
    }

    if (boneAnimIndex >= 0) {
        const BoneAnim& boneAnim = anim.bones[boneAnimIndex];
        glm::mat4 rot = calcInterpolatedRotation(time, boneAnim);
        glm::mat4 sca = calcInterpolatedScale(time, boneAnim);
        if (depth == 0) {
            nodeTransform = rot * sca;
        } else {
            glm::mat4 pos = calcInterpolatedPosition(time, boneAnim);
            nodeTransform = pos * rot * sca;
        }
    } else if (depth == 0) {
        glm::mat4 t = node.transform;
        t[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        nodeTransform = t;
    }

    glm::mat4 globalTransform = parentTransform * nodeTransform;

    if (!node.name.empty() && boneMap.find(node.name) != boneMap.end()) {
        int boneId = boneMap.at(node.name);
        if (boneId >= 0 && boneId < MAX_BONES) {
            boneTransforms[boneId] = globalInverseTransform * globalTransform * boneInfo[boneId].offset;
        }
    }

    for(size_t i = 0; i < node.children.size(); i++) {
        readNodeHierarchy(time, node.children[i], globalTransform, depth + 1);
    }
}

int Model::findBoneAnimIndex(const std::string& name) const {
    if (animations.empty()) return -1;
    const Animation& anim = animations[currentAnimIndex];
    for(size_t i = 0; i < anim.bones.size(); i++) {
        if (anim.bones[i].name == name) return (int)i;
    }
    return -1;
}

glm::mat4 Model::calcInterpolatedPosition(float time, const BoneAnim& anim) const {
    if (anim.positions.empty()) return glm::mat4(1.0f);
    if (anim.positions.size() == 1) {
        return glm::translate(glm::mat4(1.0f), anim.positions[0].pos);
    }

    size_t idx = 0;
    for(size_t i = 0; i < anim.positions.size() - 1; i++) {
        if (time < anim.positions[i + 1].time) {
            idx = i;
            break;
        }
        idx = i;
    }

    size_t nextIdx = idx + 1;
    if (nextIdx >= anim.positions.size()) nextIdx = anim.positions.size() - 1;

    float t1 = anim.positions[idx].time;
    float t2 = anim.positions[nextIdx].time;
    float factor = (t2 - t1 != 0.0f) ? (time - t1) / (t2 - t1) : 0.0f;

    glm::vec3 pos = glm::mix(anim.positions[idx].pos, anim.positions[nextIdx].pos, factor);
    return glm::translate(glm::mat4(1.0f), pos);
}

glm::mat4 Model::calcInterpolatedRotation(float time, const BoneAnim& anim) const {
    if (anim.rotations.empty()) return glm::mat4(1.0f);
    if (anim.rotations.size() == 1) {
        return glm::mat4_cast(anim.rotations[0].rot);
    }

    size_t idx = 0;
    for(size_t i = 0; i < anim.rotations.size() - 1; i++) {
        if (time < anim.rotations[i + 1].time) {
            idx = i;
            break;
        }
        idx = i;
    }

    size_t nextIdx = idx + 1;
    if (nextIdx >= anim.rotations.size()) nextIdx = anim.rotations.size() - 1;

    float t1 = anim.rotations[idx].time;
    float t2 = anim.rotations[nextIdx].time;
    float factor = (t2 - t1 != 0.0f) ? (time - t1) / (t2 - t1) : 0.0f;

    glm::quat rot = glm::slerp(anim.rotations[idx].rot, anim.rotations[nextIdx].rot, factor);
    return glm::mat4_cast(glm::normalize(rot));
}

glm::mat4 Model::calcInterpolatedScale(float time, const BoneAnim& anim) const {
    if (anim.scales.empty()) return glm::mat4(1.0f);
    if (anim.scales.size() == 1) {
        return glm::scale(glm::mat4(1.0f), anim.scales[0].scale);
    }

    size_t idx = 0;
    for(size_t i = 0; i < anim.scales.size() - 1; i++) {
        if (time < anim.scales[i + 1].time) {
            idx = i;
            break;
        }
        idx = i;
    }

    size_t nextIdx = idx + 1;
    if (nextIdx >= anim.scales.size()) nextIdx = anim.scales.size() - 1;

    float t1 = anim.scales[idx].time;
    float t2 = anim.scales[nextIdx].time;
    float factor = (t2 - t1 != 0.0f) ? (time - t1) / (t2 - t1) : 0.0f;

    glm::vec3 sca = glm::mix(anim.scales[idx].scale, anim.scales[nextIdx].scale, factor);
    return glm::scale(glm::mat4(1.0f), sca);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName) {
    std::vector<Texture> textures;
    for(unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str;
        mat->GetTexture(type, i, &str);

        bool skip = false;
        for(unsigned int j = 0; j < textures_loaded.size(); j++) {
            if(std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0) {
                textures.push_back(textures_loaded[j]);
                skip = true;
                break;
            }
        }

        if(!skip) {
            Texture texture;
            texture.id = TextureFromFile(str.C_Str(), directory);
            texture.type = typeName;
            texture.path = str.C_Str();
            textures.push_back(texture);
            textures_loaded.push_back(texture);
        }
    }
    return textures;
}

<<<<<<< HEAD
=======
// Constructor de copia
>>>>>>> 5ef2c6448ea51885101b988eaf507094d924b7a3
Model::Model(const Model& other) {
    position = other.position;
    rotation = other.rotation;
    scale = other.scale;
    directory = other.directory;
    textures_loaded = other.textures_loaded;
    meshes = other.meshes;
<<<<<<< HEAD
    boneInfo = other.boneInfo;
    boneMap = other.boneMap;
    boneCount = other.boneCount;
    animations = other.animations;
    currentAnimIndex = other.currentAnimIndex;
    animTime = other.animTime;
    animLoop = other.animLoop;
    boneTransforms = other.boneTransforms;
    globalInverseTransform = other.globalInverseTransform;
    rootNode = other.rootNode;
    std::cout << "Modelo copiado correctamente" << std::endl;
}

=======
    std::cout << "Modelo copiado correctamente" << std::endl;
}

// Operador de asignación
>>>>>>> 5ef2c6448ea51885101b988eaf507094d924b7a3
Model& Model::operator=(const Model& other) {
    if (this != &other) {
        position = other.position;
        rotation = other.rotation;
        scale = other.scale;
        directory = other.directory;
        textures_loaded = other.textures_loaded;
        meshes = other.meshes;
<<<<<<< HEAD
        boneInfo = other.boneInfo;
        boneMap = other.boneMap;
        boneCount = other.boneCount;
        animations = other.animations;
        currentAnimIndex = other.currentAnimIndex;
        animTime = other.animTime;
        animLoop = other.animLoop;
        boneTransforms = other.boneTransforms;
        globalInverseTransform = other.globalInverseTransform;
        rootNode = other.rootNode;
=======
>>>>>>> 5ef2c6448ea51885101b988eaf507094d924b7a3
    }
    return *this;
}

<<<<<<< HEAD
=======
// Destructor
>>>>>>> 5ef2c6448ea51885101b988eaf507094d924b7a3
Model::~Model() {
    std::cout << "Modelo destruido" << std::endl;
}

unsigned int Model::TextureFromFile(const char *path, const std::string &directory) {
    std::string filename = std::string(path);

    std::vector<std::string> pathsToTry;
    pathsToTry.push_back(directory + "/" + filename);
    pathsToTry.push_back(directory + "/../Textures/" + filename);
    pathsToTry.push_back("Textures/" + filename);
    pathsToTry.push_back("models/Caja/" + filename);
    pathsToTry.push_back(filename);

    unsigned int textureID = 0;
    for (const auto& fullPath : pathsToTry) {
        textureID = SOIL_load_OGL_texture(
            fullPath.c_str(),
            SOIL_LOAD_RGB,
            SOIL_CREATE_NEW_ID,
            SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y
        );

        if (textureID != 0) {
            std::cout << "Textura del modelo cargada: " << fullPath << std::endl;
            break;
        }
    }

    if (textureID == 0) {
        std::cout << "ERROR: No se pudo cargar textura: " << path << std::endl;
        unsigned char defaultTexture[] = {255, 255, 255, 255};
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, defaultTexture);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    return textureID;
<<<<<<< HEAD
}
=======
}
>>>>>>> 5ef2c6448ea51885101b988eaf507094d924b7a3
