#include "Object.h"
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <filesystem>
#include <iostream>
#include <fstream>
#include <sstream>
#include <format>

Object::Object(std::string const& path, bool flipTextures, Shader& shader) : shaderptr(shader)
{
	stbi_set_flip_vertically_on_load(flipTextures);
	loadModel(path);
	Position = glm::vec3(0.0f, 0.0f, 0.0f);
	Scale = glm::vec3(1.0f, 1.0f, 1.0f);
	Translate(Position);
	SetScale(Scale);
}


void Object::AddTexture(const char* texturePath)
{
	texture.push_back(0);

	glGenTextures(1, &texture[texture.size() - 1]);
	glBindTexture(GL_TEXTURE_2D, texture[texture.size() - 1]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	stbi_set_flip_vertically_on_load(true);

	int width, height, nrChannels;
	unsigned char* data = stbi_load(texturePath, &width, &height, &nrChannels, 4);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		std::cout << "Texture loaded successfully: " << texturePath << std::endl;
	}
	else
	{
		std::cout << "Failed to load texture: " << texturePath << std::endl;
	}
	stbi_image_free(data);

	std::string TextureAttribute = std::format("texture_diffuse{0}", texture.size() - 1);

	textureLocation.push_back(shaderptr.GetUniformLocation(TextureAttribute));

	glUniform1i(textureLocation[texture.size() - 1], texture.size() - 1);
}

void Object::Draw(Shader& shader)
{
	shaderptr.SetUniformMat4f("model", modelMatrix);

	for (unsigned int i = 0; i < meshes.size(); i++)
		meshes[i].Draw(shader);

}


void Object::loadModel(std::string path)
{

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
	{
		std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
		return;
	}

	directory = path.substr(0, path.find_last_of('/'));

	processNode(scene->mRootNode, scene);
}

void Object::processNode(aiNode* ainode, const aiScene* aiscene)
{
	for (unsigned int i = 0; i < ainode->mNumMeshes; i++)
	{
		aiMesh* mesh = aiscene->mMeshes[ainode->mMeshes[i]];
		meshes.push_back(processMesh(mesh, aiscene));
	}

	for (unsigned int i = 0; i < ainode->mNumChildren; i++)
	{
		processNode(ainode->mChildren[i], aiscene);
	}
}

Mesh Object::processMesh(aiMesh* aimesh, const aiScene* aiscene)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	// walk through each of the mesh's vertices
	for (unsigned int i = 0; i < aimesh->mNumVertices; i++)
	{
		Vertex vertex;
		glm::vec3 vector;

		vector.x = aimesh->mVertices[i].x;
		vector.y = aimesh->mVertices[i].y;
		vector.z = aimesh->mVertices[i].z;
		vertex.Position = vector;

		if (aimesh->HasNormals())
		{
			vector.x = aimesh->mNormals[i].x;
			vector.y = aimesh->mNormals[i].y;
			vector.z = aimesh->mNormals[i].z;
			vertex.Normal = vector;
		}

		if (aimesh->mTextureCoords[0])
		{
			glm::vec2 vec;
			vec.x = aimesh->mTextureCoords[0][i].x;
			vec.y = aimesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;
		}
		else
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);

		vertices.push_back(vertex);
	}

	for (unsigned int i = 0; i < aimesh->mNumFaces; i++)
	{
		aiFace face = aimesh->mFaces[i];

		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	aiMaterial* material = aiscene->mMaterials[aimesh->mMaterialIndex];


	std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
	textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());


	return Mesh(vertices, indices, textures, shaderptr);
}

std::vector<Texture> Object::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)

{
	std::vector<Texture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);

		bool skip = false;
		for (unsigned int j = 0; j < textures_loaded.size(); j++)
		{
			if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
			{
				textures.push_back(textures_loaded[j]);
				skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
				break;
			}
		}
		if (!skip)
		{   // if texture hasn't been loaded already, load it
			Texture texture;
			texture.id = TextureFromFile(str.C_Str(), this->directory);
			texture.type = typeName;
			texture.path = str.C_Str();
			textures.push_back(texture);
			textures_loaded.push_back(texture);
		}
	}
	return textures;
}

unsigned int Object::TextureFromFile(const char* path, const std::string& directory, bool gamma)
{
	std::string filename = std::string(path);
	filename = directory + '/' + filename;

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		std::cout << "Texture loaded successfully: " << filename << std::endl;
		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << filename << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

void Object::Translate(glm::vec3 newPos)
{
	Position = newPos;
	modelMatrix = glm::translate(modelMatrix, Position);
}

void Object::AddToPosition(glm::vec3 vectorToAdd)
{
	Position += vectorToAdd;
	modelMatrix = glm::translate(modelMatrix, Position);
}

void Object::SetScale(glm::vec3 newScale)
{
	Scale = newScale;
	modelMatrix = glm::scale(modelMatrix, Scale);
}

void Object::SetRotation(glm::vec3 _RotateAxis, float _rotationValue)
{
	RotationAxis = _RotateAxis;
	RotationValue = _rotationValue;

	modelMatrix = glm::rotate(modelMatrix, RotationValue, RotationAxis);
}
