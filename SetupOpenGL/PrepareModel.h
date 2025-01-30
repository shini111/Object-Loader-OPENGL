#ifndef Prep_Model
#define Prep_Model

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp> 
#include "Collider.h"
#include "OBJLoader.h"

struct ObjectData {
    std::vector<glm::vec4> vertices;
    std::vector<glm::vec3> normals;
    std::vector<GLushort> elements;
    std::vector<glm::vec2> texcoord;
    glm::mat4 model;
    CollisionHelper helper;
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
    GLuint texture;
    bool isStaticValue;
};


void prep_obj(const char* filename, ObjectData& objectData, glm::vec3& position, glm::vec3& size, const char* texturePath, bool isStatic);

#endif // Prep_Model#pragma once
