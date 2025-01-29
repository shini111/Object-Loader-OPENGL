#ifndef OBJECT_H
#define OBJECT_H

#include <vector>
#include <string>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Mesh.h"
#include "Shader.h"
#include "stb_image.h"

class Object {
public:
    // Constructor
    Object(const std::string& path, bool flipTextures, Shader& shader);

    // Public Methods
    void Draw(Shader& shader);
    void AddTexture(const char* texturePath);

    // Transformations
    void Translate(glm::vec3 newPos);
    void AddToPosition(glm::vec3 vectorToAdd);
    void SetScale(glm::vec3 newScale);
    void SetRotation(glm::vec3 _RotateAxis, float _rotationValue);

    // Texture Handling
    unsigned int getTextureID(size_t index = 0) const { return (index < texture.size()) ? texture[index] : 0; }
    size_t getTextureCount() const { return texture.size(); }

    // Debugging
    bool isLoadedSuccessfully() const { return loadedSuccessfully; }

private:
    // Model Data
    std::vector<Mesh> meshes;
    std::vector<unsigned int> texture;
    std::vector<int> textureLocation;
    std::string directory;

    // Shader
    Shader& shaderptr;

    // Transformation Data
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    glm::vec3 Position;
    glm::vec3 Scale;
    glm::vec3 RotationAxis;
    float RotationValue = 0.0f;

    // OBJ Loader
    void loadModel(std::string path);
    void processMeshData(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices);
    void parseOBJFile(const std::string& path);

    // Texture Loading
    unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma = false);

    // Load Status
    bool loadedSuccessfully = false;

    void loadMaterial(std::string path);
    std::vector<Texture> getMaterialTextures(std::string materialName);
    unsigned int TextureFromFile(const char* path, const std::string& directory)
};

#endif
