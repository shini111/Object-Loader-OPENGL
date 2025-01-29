#include "Object.h"
#include "OBJLoader.h"
#include <iostream>


// In Object.cpp
Object::Object(const std::string& path, Shader& shader) : shaderptr(shader) {  // Definition
    loadModel(path);


}

void Object::loadModel(const std::string& path) {
    std::vector<Vertex> m_vertices;
    std::vector<unsigned int> m_indices;
    std::vector<Texture> m_textures;  // Assuming you want to load textures too

    if (LoadOBJ(path, m_vertices, m_indices, m_textures)) {
        meshes.push_back(Mesh(m_vertices, m_indices, m_textures, shaderptr));
    }
    else {
        std::cerr << "Failed to load model: " << path << std::endl;
    }
}

void Object::Draw(Shader& shader) {
    shaderptr.SetUniformMat4f("model", modelMatrix);
    for (unsigned int i = 0; i < meshes.size(); i++) {
        meshes[i].Draw(shader);
    }
}

void Object::Translate(glm::vec3 newPos) {
    modelMatrix = glm::translate(modelMatrix, newPos);
}

void Object::SetScale(glm::vec3 newScale) {
    modelMatrix = glm::scale(modelMatrix, newScale);
}

void Object::SetRotation(glm::vec3 axis, float value) {
    modelMatrix = glm::rotate(modelMatrix, value, axis);
}
