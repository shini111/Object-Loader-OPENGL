#include "OBJLoader.h"
#include <fstream>
#include <sstream>
#include <iostream>

MeshData OBJLoader::LoadOBJ(const std::string& filePath) {
    MeshData mesh;
    std::vector<glm::vec3> temp_positions;
    std::vector<glm::vec2> temp_texCoords;
    std::vector<glm::vec3> temp_normals;

    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Failed to open OBJ file: " << filePath << std::endl;
        return mesh;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream stream(line);
        std::string prefix;
        stream >> prefix;

        if (prefix == "v") {  // Vertex position
            glm::vec3 position;
            stream >> position.x >> position.y >> position.z;
            temp_positions.push_back(position);
        }
        else if (prefix == "vt") {  // Texture coordinate
            glm::vec2 texCoord;
            stream >> texCoord.x >> texCoord.y;
            temp_texCoords.push_back(texCoord);
        }
        else if (prefix == "vn") {  // Normal
            glm::vec3 normal;
            stream >> normal.x >> normal.y >> normal.z;
            temp_normals.push_back(normal);
        }
        else if (prefix == "f") {  // Face
            unsigned int vIndex[3], tIndex[3], nIndex[3];

            char slash;
            for (int i = 0; i < 3; i++) {
                stream >> vIndex[i] >> slash >> tIndex[i] >> slash >> nIndex[i];

                Vertex vertex;
                vertex.Position = temp_positions[vIndex[i] - 1];
                vertex.TexCoords = temp_texCoords[tIndex[i] - 1];
                vertex.Normal = temp_normals[nIndex[i] - 1];

                mesh.vertices.push_back(vertex);
                mesh.indices.push_back(mesh.vertices.size() - 1);
            }
        }
    }

    std::cout << "OBJ Loaded: " << filePath
        << " | Vertices: " << mesh.vertices.size()
        << " | Faces: " << mesh.indices.size() / 3 << std::endl;

    return mesh;
}
