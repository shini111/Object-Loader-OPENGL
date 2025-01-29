#include "OBJLoader.h"
#include <iostream>
#include <fstream>
#include <sstream>

bool LoadOBJ(const std::string& path, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, std::vector<Texture>& textures)
{
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open OBJ file: " << path << std::endl;
        return false;
    }

    std::vector<glm::vec3> temp_vertices;
    std::vector<glm::vec2> temp_texCoords;
    std::vector<glm::vec3> temp_normals;

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string prefix;
        ss >> prefix;

        if (prefix == "v") {
            glm::vec3 vertex;
            ss >> vertex.x >> vertex.y >> vertex.z;
            temp_vertices.push_back(vertex);
        }
        else if (prefix == "vt") {
            glm::vec2 texCoord;
            ss >> texCoord.x >> texCoord.y;
            temp_texCoords.push_back(texCoord);
        }
        else if (prefix == "vn") {
            glm::vec3 normal;
            ss >> normal.x >> normal.y >> normal.z;
            temp_normals.push_back(normal);
        }
        else if (prefix == "f") {
            std::string vertex1, vertex2, vertex3;
            unsigned int vIndex[3], tIndex[3], nIndex[3];
            char slash;

            ss >> vertex1 >> vertex2 >> vertex3;
            // Parse the face data
            for (int i = 0; i < 3; i++) {
                std::istringstream ss_v(vertex1);
                ss_v >> vIndex[i] >> slash >> tIndex[i] >> slash >> nIndex[i];
                indices.push_back(vIndex[i] - 1); // OBJ indices are 1-based
            }
        }
    }

    // Now that we have all vertices, normals, and texture coords, we need to build the final vertices.
    for (unsigned int i = 0; i < indices.size(); i++) {
        Vertex vertex;
        unsigned int vertexIndex = indices[i];
        vertex.Position = temp_vertices[vertexIndex];

        if (vertexIndex < temp_texCoords.size()) {
            vertex.TexCoords = temp_texCoords[vertexIndex];
        }
        else {
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);
        }

        if (vertexIndex < temp_normals.size()) {
            vertex.Normal = temp_normals[vertexIndex];
        }
        else {
            vertex.Normal = glm::vec3(0.0f, 0.0f, 0.0f);
        }

        vertices.push_back(vertex);
    }

    return true;
}

// Function to load texture from file
unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma)
{
    std::string filename = directory + '/' + std::string(path);
    unsigned int textureID;
    glGenTextures(1, &textureID);
    int width, height, nrChannels;
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);

    if (data) {
        GLenum format = (nrChannels == 1) ? GL_RED : (nrChannels == 3) ? GL_RGB : GL_RGBA;
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);
    }
    else {
        std::cerr << "Failed to load texture: " << filename << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}
