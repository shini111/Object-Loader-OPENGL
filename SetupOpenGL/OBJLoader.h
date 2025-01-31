#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <glm/glm.hpp>
#include "Mesh.h"

class OBJLoader 
{
public:
    // Structure representing a face (polygon)
    struct Face {
        std::vector<unsigned int> vertexIndices;
        std::vector<unsigned int> texCoordIndices;
        std::vector<unsigned int> normalIndices;
    };

    // Structure for material properties
    struct Material {
        std::string name;
        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;
        float shininess;
        std::string texturePath;
    };

    // Structure for an object in the OBJ file
    struct ObjectInfo {
        std::string name;
        std::vector<Face> faces;
        std::string material;
    };

    // Structure to store all OBJ data
    struct OBJData {
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec2> texCoords;
        std::vector<glm::vec3> normals;
        std::unordered_map<std::string, Material> materials;
        std::vector<ObjectInfo> objects;
    };

    // Function to load an OBJ file and parse its contents
    bool LoadOBJ(const std::string& path, OBJData& objData);

};


