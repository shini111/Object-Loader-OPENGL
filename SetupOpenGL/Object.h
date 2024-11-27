#pragma once
#include "Mesh.h"
#include "Shader.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <Assimp/Importer.hpp>
#include <Assimp/scene.h>
#include <Assimp/postprocess.h>

class Object
{
public:
	Object(std::string const& path, bool flipTextures, Shader& shader);
	void AddTexture(const char* texturePath);
	void Draw(Shader& shader);
	void Translate(glm::vec3 newPos);
	void AddToPosition(glm::vec3 vectorToAdd);
	void SetScale(glm::vec3 newScale);
	void SetRotation(glm::vec3 RotateAxis, float rotationValue);

private:

	Shader& shaderptr;
	std::vector<Mesh> meshes;
	std::string directory;

	void loadModel(std::string path);
	void processNode(aiNode* ainode, const aiScene* aiscene);
	Mesh processMesh(aiMesh* aimesh, const aiScene* aiscene);
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type,
		std::string typeName);
	unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma = false);

	void ResetMatrix() { modelMatrix = glm::mat4(1.0f); }


	std::vector<Texture> textures_loaded;

	std::string modelName;

	GLint positionAttribute;
	GLint textureCoordAttribute;
	GLint colorAttribute;

	std::vector<GLuint> texture;
	std::vector <GLuint> textureLocation;

	glm::vec3 Position;
	glm::vec3 Scale;
	glm::vec3 RotationAxis;
	float RotationValue;

	GLuint modelAttribute;
	glm::mat4 modelMatrix = glm::mat4(1.0f);

};