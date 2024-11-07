#pragma once
#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

class Object {
public:
	Object(const char* filename, GLuint shaderProgram);
	void Draw(GLuint viewLocation, GLuint projectionLocation, glm::mat4 view, glm::mat4 projection, glm::mat4 translatevalue);

private:
	std::vector<glm::vec4> vertices;
	std::vector<glm::vec3> normals;
	std::vector<GLushort> elements;
	std::vector<glm::vec2> texCoords;

	GLuint vao, vbo, ebo, texCoordVBO;
	GLuint shaderProgram;
	GLuint texture;
	GLuint modelLocation;

	void initBuffers();
	void loadTextures(const char* textureFile);
	void load_obj(const char* filename);
};