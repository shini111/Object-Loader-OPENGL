#ifndef OBJECT_H
#define OBJECT_H

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "Shader.h"
#include "Mesh.h"
#include "stb_image.h"


class Object {
public:
    Object(const std::string& path, Shader& shader);

    void loadModel(const std::string& path);
    void Draw(Shader& shader);

    glm::mat4 modelMatrix = glm::mat4(1.0f);
    void Translate(glm::vec3 newPos);
    void SetScale(glm::vec3 newScale);
    void SetRotation(glm::vec3 axis, float value);

    unsigned int LoadTexture(const std::string& path);

private:
    std::vector<Mesh> meshes;
    Shader shaderptr;
};

#endif
