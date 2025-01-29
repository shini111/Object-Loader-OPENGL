#ifndef MESH_H
#define MESH_H

#include <vector>
#include "Shader.h"
#include <glm/glm.hpp>

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

struct Texture {
    unsigned int id;
    std::string type;
    std::string path;
};

class Mesh {
public:
    Mesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, std::vector<Texture>& textures, Shader& shader);
    void Draw(Shader& shader);  // Render the mesh using OpenGL

private:
    unsigned int VAO, VBO, EBO;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    void setupMesh();  // Setup OpenGL buffers and arrays
};

#endif
