#pragma once
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include "Mesh.h"  // Include Mesh to use Vertex struct

struct MeshData {
    std::vector<Vertex> vertices;  // Use existing Vertex struct from Mesh.h
    std::vector<unsigned int> indices;
};

class OBJLoader {
public:
    static MeshData LoadOBJ(const std::string& filePath);
};
