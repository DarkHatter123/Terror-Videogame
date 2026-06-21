#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Mesh.h"
#include "Shader.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>

#define MAX_BONES 100

struct BoneInfo {
    int id;
    std::string name;
    glm::mat4 offset;
};

struct BoneKeyPos { float time; glm::vec3 pos; };
struct BoneKeyRot { float time; glm::quat rot; };
struct BoneKeyScale { float time; glm::vec3 scale; };

struct BoneAnim {
    std::string name;
    int boneId;
    std::vector<BoneKeyPos> positions;
    std::vector<BoneKeyRot> rotations;
    std::vector<BoneKeyScale> scales;
};

struct Animation {
    std::string name;
    float duration;
    float ticksPerSecond;
    std::vector<BoneAnim> bones;
};

struct NodeData {
    std::string name;
    glm::mat4 transform;
    std::vector<NodeData> children;
};

class Model {
public:
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    
    Model(const char* path);
<<<<<<< HEAD
    Model(const Model& other);
    Model& operator=(const Model& other);
    ~Model();

    void Draw(Shader &shader);
    void DrawAnimated(Shader &shader);
    void setManualTexture(const char* texturePath);
    
    void setAnimation(float time);
    void updateAnimation(float deltaTime);
    bool hasAnimation() const { return !animations.empty(); }
    float getAnimationDuration() const;
    int getMeshCount() const { return (int)meshes.size(); }
    int getBoneCount() const { return boneCount; }
=======
    Model(const Model& other);  // ← Constructor de copia
    Model& operator=(const Model& other);  // ← Operador de asignación
    ~Model();  // ← Destructor

    void Draw(Shader &shader);
    void setManualTexture(const char* texturePath);
>>>>>>> 5ef2c6448ea51885101b988eaf507094d924b7a3
    
private:
    std::vector<Mesh> meshes;
    std::string directory;
    std::vector<Texture> textures_loaded;
    
    std::vector<BoneInfo> boneInfo;
    std::unordered_map<std::string, int> boneMap;
    int boneCount;
    
    std::vector<Animation> animations;
    int currentAnimIndex;
    float animTime;
    bool animLoop;
    std::vector<glm::mat4> boneTransforms;
    glm::mat4 globalInverseTransform;

    NodeData rootNode;
    
    void loadModel(std::string path);
    void processNode(aiNode *node, const aiScene *scene);
    NodeData buildNodeTree(aiNode* node);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);
    void extractBoneData(aiMesh* mesh, std::vector<int>& boneIds, std::vector<float>& weights);
    void loadAnimations(const aiScene* scene);
    void computeBoneTransforms(float time);
    void readNodeHierarchy(float time, const NodeData& node, const glm::mat4& parentTransform, int depth = 0);
    int findBoneAnimIndex(const std::string& name) const;
    glm::mat4 calcInterpolatedPosition(float time, const BoneAnim& anim) const;
    glm::mat4 calcInterpolatedRotation(float time, const BoneAnim& anim) const;
    glm::mat4 calcInterpolatedScale(float time, const BoneAnim& anim) const;
    
    std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
    unsigned int TextureFromFile(const char *path, const std::string &directory);
};
