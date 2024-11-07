#include "Object.h"
#include "stb_image.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <fstream>
#include <sstream>

Object::Object(const char* filename, GLuint shaderProgram) : shaderProgram(shaderProgram) {
	load_obj(filename);
	initBuffers();
	loadTextures("models/Cube/Material_BaseColor.bmp");
	modelLocation = glGetUniformLocation(shaderProgram, "model");
}

void Object::initBuffers() {
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements.size() * sizeof(elements), &elements[0], GL_STATIC_DRAW);

	GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)0);

	if (!texCoords.empty()) {
		glGenBuffers(1, &texCoordVBO);
		glBindBuffer(GL_ARRAY_BUFFER, texCoordVBO);
		glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(glm::vec2), &texCoords[0], GL_STATIC_DRAW);

		GLint texCoordAttrib = glGetAttribLocation(shaderProgram, "texCoord");
		glEnableVertexAttribArray(texCoordAttrib);
		glVertexAttribPointer(texCoordAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
	}
}

void Object::loadTextures(const char* textureFile) {
	int textureWidth, textureHeight, textureChannels;
	unsigned char* textureData = stbi_load(textureFile, &textureWidth, &textureHeight, &textureChannels, 0);
	if (!textureData) {
		std::cerr << "Failed to load texture image." << std::endl;
		return;
	}

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight , 0, GL_RGB, GL_UNSIGNED_BYTE, textureData);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(textureData);
}

void Object::load_obj(const char* filename) {
	std::ifstream in(filename, std::ios::in);
	if (!in) {
		std::cerr << "Cannot open " << filename << std::endl;
		exit(1);
	}

	std::string line;
	while (getline(in, line)) {
		if (line.substr(0, 2) == "v ") {
			std::istringstream s(line.substr(2));
			glm::vec4 v;
			s >> v.x;
			s >> v.y;
			s >> v.z;
			v.w = 1.0f;
			vertices.push_back(v);
		}
		else if (line.substr(0, 2) == "f ") {
			std::istringstream s(line.substr(2));
			GLushort a, b, c, ign;
			s >> a;
			s.ignore();
			s >> ign;
			s.ignore();
			s >> ign;

			s >> b;
			s.ignore();
			s >> ign;
			s.ignore();
			s >> ign;

			s >> c;
			a--;
			b--;
			c--;
			elements.push_back(a);
			elements.push_back(b);
			elements.push_back(c);
		}
		else if (line.substr(0, 3) == "vt ") {
			std::istringstream s(line.substr(3));
			glm::vec2 vt;
			s >> vt.x;
			s >> vt.y;
			texCoords.push_back(vt);
		}
	}

	normals.resize(vertices.size(), glm::vec3(0.0, 0.0, 0.0));
	for (size_t i = 0; i < elements.size(); i += 3) {
		GLushort ia = elements[i];
		GLushort ib = elements[i + 1];
		GLushort ic = elements[i + 2];
		if (ia < vertices.size() && ib < vertices.size() && ic < vertices.size()) {
			glm::vec3 normal = glm::normalize(glm::cross(glm::vec3(vertices[ib]) - glm::vec3(vertices[ia]), glm::vec3(vertices[ic]) - glm::vec3(vertices[ia])));
			normals[ia] = normals[ib] = normals[ic] = normal;
		}
	}
}

void Object::Draw(GLuint viewLocation, GLuint projectionLocation, glm::mat4 view, glm::mat4 projection, glm::mat4 translatevalue) {
	glUseProgram(shaderProgram);

	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glBindVertexArray(vao);

	glm::mat4 model(1.0f);
	model = translatevalue;

	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

	glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(elements.size()), GL_UNSIGNED_SHORT, 0);
}