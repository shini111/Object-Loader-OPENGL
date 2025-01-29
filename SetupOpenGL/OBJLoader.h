#ifndef OBJLOADER_H
#define OBJLOADER_H

#include <vector>
#include <string>
#include "Mesh.h"
#include "stb_image.h"
#include <glm/glm.hpp>
#include "Mesh.h"


// Function to load OBJ file
bool LoadOBJ(const std::string& path, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, std::vector<Texture>& textures);

// Function to load textures from files
unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma = false);

#endif
