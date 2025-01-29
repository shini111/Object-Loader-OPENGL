#include "Object.h"
#include "OBJLoader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <algorithm>

Object::Object(std::string const& path, bool flipTextures, Shader& shader) : shaderptr(shader) {
    stbi_set_flip_vertically_on_load(flipTextures);
    loadModel(path);
    Position = glm::vec3(0.0f, 0.0f, 0.0f);
    Scale = glm::vec3(1.0f, 1.0f, 1.0f);
    Translate(Position);
    SetScale(Scale);
}

void Object::loadModel(std::string path) {
    MeshData meshData = OBJLoader::LoadOBJ(path);

    if (meshData.vertices.empty()) {
        std::cerr << "Error: Failed to load model from " << path << std::endl;
        loadedSuccessfully = false;
        return;
    }

    meshes.push_back(Mesh(meshData.vertices, meshData.indices, {}));
    loadedSuccessfully = true;
}

void Object::loadMaterial(std::string path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "ERROR::OBJLOADER::Could not open MTL file: " << path << std::endl;
        return;
    }

    std::string line;
    std::string currentMaterial;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string prefix;
        ss >> prefix;

        if (prefix == "newmtl") { // New Material
            ss >> currentMaterial;
            materials[currentMaterial] = Material();
        }
        else if (prefix == "map_Kd") { // Diffuse Texture
            std::string textureFile;
            ss >> textureFile;
            materials[currentMaterial].diffuseMap = TextureFromFile(textureFile.c_str(), path.substr(0, path.find_last_of('/')));
            materials[currentMaterial].hasDiffuse = true;
        }
        else if (prefix == "map_Ks") { // Specular Texture
            std::string textureFile;
            ss >> textureFile;
            materials[currentMaterial].specularMap = TextureFromFile(textureFile.c_str(), path.substr(0, path.find_last_of('/')));
            materials[currentMaterial].hasSpecular = true;
        }
    }

    file.close();
}

std::vector<Texture> Object::getMaterialTextures(std::string materialName) {
    std::vector<Texture> textures;
    if (materials.find(materialName) != materials.end()) {
        if (materials[materialName].hasDiffuse) {
            textures.push_back({ materials[materialName].diffuseMap, "texture_diffuse", "" });
        }
        if (materials[materialName].hasSpecular) {
            textures.push_back({ materials[materialName].specularMap, "texture_specular", "" });
        }
    }
    return textures;
}

unsigned int Object::TextureFromFile(const char* path, const std::string& directory) {
    std::string filename = directory + "/" + std::string(path);
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        std::cout << "Texture loaded successfully: " << filename << std::endl;
    }
    else {
        std::cerr << "Texture failed to load: " << filename << std::endl;
    }
    stbi_image_free(data);
    return textureID;
}
