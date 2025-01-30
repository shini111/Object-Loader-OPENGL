#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <glm/glm.hpp>
#include "OBJLoader.h"

std::string modelPath = "Models/Med/";

bool LoadOBJ(const std::string& path, OBJData& objData) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open OBJ file: " << path << std::endl;
        return false;
    }

    std::string line;
    ObjectInfo currentObject;
    std::string currentMaterial;
    bool hasObject = false;

    while (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string prefix;
        ss >> prefix;

        if (prefix == "v") {
            glm::vec3 vertex;
            ss >> vertex.x >> vertex.y >> vertex.z;
            objData.vertices.push_back(vertex);
        }
        else if (prefix == "vt") {
            glm::vec2 texCoord;
            ss >> texCoord.x >> texCoord.y;
            texCoord.y = 1.0f - texCoord.y; // Flip V for OpenGL
            objData.texCoords.push_back(texCoord);
        }
        else if (prefix == "vn") {
            glm::vec3 normal;
            ss >> normal.x >> normal.y >> normal.z;
            objData.normals.push_back(normal);
        }
        else if (prefix == "f") {
            Face face;
            std::string vertexData;
            while (ss >> vertexData) {
                std::istringstream vss(vertexData);
                unsigned int vIndex, tIndex = 0, nIndex = 0;
                char slash;

                vss >> vIndex;
                if (vss.peek() == '/') {
                    vss.get();
                    if (vss.peek() != '/') vss >> tIndex;
                    if (vss.peek() == '/') {
                        vss.get();
                        vss >> nIndex;
                    }
                }

                face.vertexIndices.push_back(vIndex - 1);
                if (tIndex > 0) face.texCoordIndices.push_back(tIndex - 1);
                if (nIndex > 0) face.normalIndices.push_back(nIndex - 1);
            }

            if (!hasObject) {
                currentObject.name = "default";
                hasObject = true;
            }

            currentObject.faces.push_back(face);
        }
        else if (prefix == "o") {
            if (hasObject) objData.objects.push_back(currentObject);
            currentObject = ObjectInfo();
            ss >> currentObject.name;
            hasObject = true;
        }
        else if (prefix == "g") {
            if (hasObject) objData.objects.push_back(currentObject);
            currentObject = ObjectInfo();
            ss >> currentObject.name;
            hasObject = true;
        }
        else if (prefix == "usemtl") {
            ss >> currentMaterial;
            currentObject.material = currentMaterial;
        }
        else if (prefix == "mtllib") {
            std::string mtlFilename;
            ss >> mtlFilename;

            // Extract directory from OBJ file path
            std::string objDirectory = path.substr(0, path.find_last_of("/\\") + 1);

            // Construct the full path to the MTL file
            std::string mtlFilePath = objDirectory + mtlFilename;
            //std::cout << mtlFilePath << std::endl;
            std::ifstream mtlStream(mtlFilePath);
            if (!mtlStream.is_open()) {
                std::cerr << "Failed to open MTL file: " << mtlFilePath << std::endl;
                return false;
            }

            std::string mtlLine;
            Material currentMaterial;
            bool hasMaterial = false;

            while (std::getline(mtlStream, mtlLine)) {
                std::istringstream mss(mtlLine);
                std::string mtlPrefix;
                mss >> mtlPrefix;

                if (mtlPrefix == "newmtl") {
                    if (hasMaterial) objData.materials[currentMaterial.name] = currentMaterial;
                    mss >> currentMaterial.name;
                   // std::cout << "Texture name: " << currentMaterial.name << std::endl;
                    hasMaterial = true;
                }
                else if (mtlPrefix == "Ka") mss >> currentMaterial.ambient.r >> currentMaterial.ambient.g >> currentMaterial.ambient.b;
                else if (mtlPrefix == "Kd") mss >> currentMaterial.diffuse.r >> currentMaterial.diffuse.g >> currentMaterial.diffuse.b;
                else if (mtlPrefix == "Ks") mss >> currentMaterial.specular.r >> currentMaterial.specular.g >> currentMaterial.specular.b;
                else if (mtlPrefix == "Ns") mss >> currentMaterial.shininess;
                else if (mtlPrefix == "map_Kd")
                {
                    mss >> currentMaterial.texturePath;

                    // Prepend the directory of the OBJ file to the texture path
                    if (!currentMaterial.texturePath.empty()) {
                        currentMaterial.texturePath = objDirectory + currentMaterial.texturePath;
                    }

                    // Debug output to verify the full texture path
                    std::cout << "Full texture path: " << currentMaterial.texturePath << std::endl;
                }
            }

            if (hasMaterial) objData.materials[currentMaterial.name] = currentMaterial;
        }
    }

    if (hasObject) objData.objects.push_back(currentObject);
    return true;
}
