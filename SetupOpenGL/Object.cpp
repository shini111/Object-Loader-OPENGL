#include "Object.h"
#include "OBJLoader.h"
#include <iostream>

Object::Object(const std::string& path,bool flip, Shader& shader) : shaderptr(shader), toFlip(flip)
{
    loadModel(path);
}

void Object::loadModel(const std::string& path) {
    OBJData objData;

    if (!LoadOBJ(path, objData)) {
        std::cerr << "Failed to load OBJ file: " << path << std::endl;
        return;
    }

    // Process each object in the OBJ file
    for (const auto& object : objData.objects) {
        std::unordered_map<std::string, std::vector<Face>> materialToFaces;

        // Group faces by material
        for (const auto& face : object.faces) {
            materialToFaces[object.material].push_back(face);
        }

        for (const auto& [materialName, faces] : materialToFaces) {
            std::vector<Vertex> vertices;
            std::vector<unsigned int> indices;
            std::vector<Texture> textures;

            auto materialIt = objData.materials.find(materialName);
            if (materialIt != objData.materials.end()) {
                Material& material = materialIt->second;
                if (!material.texturePath.empty()) {
                    Texture texture;
                    texture.id = LoadTexture(material.texturePath);
                    texture.type = "texture_1";
                    texture.path = material.texturePath;
                    textures.push_back(texture);
                    std::cout << "Applying texture: " << material.texturePath << " for material: " << materialName << std::endl;
                }
            }

            // Convert faces to vertex data
            for (const auto& face : faces) {
                for (size_t i = 0; i < face.vertexIndices.size(); i++) {
                    Vertex vertex;
                    vertex.Position = objData.vertices[face.vertexIndices[i]];

                    if (i < face.texCoordIndices.size()) {
                        vertex.TexCoords = objData.texCoords[face.texCoordIndices[i]];
                    }
                    else {
                        vertex.TexCoords = glm::vec2(0.0f, 0.0f);
                    }

                    if (i < face.normalIndices.size()) {
                        vertex.Normal = objData.normals[face.normalIndices[i]];
                    }
                    else {
                        vertex.Normal = glm::vec3(0.0f, 0.0f, 0.0f);
                    }

                    vertices.push_back(vertex);
                    indices.push_back(static_cast<unsigned int>(vertices.size()) - 1);
                }
            }

            meshes.emplace_back(vertices, indices, textures, shaderptr);
        }
    }
}


void Object::Draw(Shader& shader) {
    shaderptr.SetUniformMat4f("model", modelMatrix);
    for (auto& mesh : meshes) {
        mesh.Draw(shader);
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

unsigned int Object::LoadTexture(const std::string& path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Load image data
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(toFlip);
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    //std::cout << stbi_failure_reason() << std::endl;
    if (data)
    {
        GLenum format;
        if (nrChannels == 1) format = GL_RED;
        else if (nrChannels == 3) format = GL_RGB;
        else if (nrChannels == 4) format = GL_RGBA;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else
    {
        //std::cerr << "Failed to load texture: " << path << std::endl;
    }

    stbi_image_free(data);
    return textureID;
}
