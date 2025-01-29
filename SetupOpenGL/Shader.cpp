#include "Shader.h"


// Definition of the static member
unsigned int Shader::program = 0;

Shader::Shader(const std::string& filepath)
	: m_FilePath(filepath), m_RendererID(0) {
	ShaderProgramSource source = ParseShader(m_FilePath);
	m_RendererID = CreateShader(source.VertexSource, source.FragmentSource);

	// Inside the Shader class constructor
	GLuint modelLoc = glGetUniformLocation(m_RendererID, "model");
	GLuint viewLoc = glGetUniformLocation(m_RendererID, "view");
	GLuint projectionLoc = glGetUniformLocation(m_RendererID, "projection");

	if (modelLoc == -1 || viewLoc == -1 || projectionLoc == -1) {
		std::cout << "Error: One or more uniforms could not be found in the shader!" << std::endl;
	}
}

Shader::~Shader() {
	glDeleteProgram(m_RendererID);
}

void Shader::Bind() {
	glUseProgram(m_RendererID);
}

void Shader::Unbind() {
	glUseProgram(0);
}

void Shader::SetUniform1i(const std::string& name, int value) {
	glUniform1i(GetUniformLocation(name), value);
}

void Shader::SetUniform3f(const std::string& name, float v0, float v1, float v2) {
	glUniform3f(GetUniformLocation(name), v0, v1, v2);
}

void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3) {
	glUniform4f(GetUniformLocation(name), v0, v1, v2, v3);
}

void Shader::SetUniformMat4f(const std::string& name, const glm::mat4& matrix) {
	glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(matrix));
}

GLint Shader::GetAttribLocation(const std::string& name) const
{
	return glGetAttribLocation(m_RendererID, name.c_str());
}

GLuint Shader::GetUniformLocation(const std::string& name) const {
	if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
		return m_UniformLocationCache[name];

	GLuint location = glGetUniformLocation(m_RendererID, name.c_str());
	if (location == -1)
		std::cout << "Warning: uniform '" << name << "' doesn't exist!" << std::endl;
	m_UniformLocationCache[name] = location;
	return location;
}

ShaderProgramSource Shader::ParseShader(const std::string& filepath) {
	std::ifstream stream(filepath);

	enum class ShaderType {
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};

	std::string line;
	std::stringstream ss[2];
	ShaderType type = ShaderType::NONE;
	while (getline(stream, line)) {
		if (line.find("#shader") != std::string::npos) {
			if (line.find("vertex") != std::string::npos)
				type = ShaderType::VERTEX;
			else if (line.find("fragment") != std::string::npos)
				type = ShaderType::FRAGMENT;
		}
		else {
			ss[(int)type] << line << '\n';
		}
	}
	return { ss[0].str(), ss[1].str() };
}

unsigned int Shader::CompileShader(unsigned int type, const std::string& source) {
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE) {
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)_malloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl;
		std::cout << message << std::endl;
		glDeleteShader(id);
		return 0;
	}

	return id;
}

unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {
	m_RendererID = glCreateProgram();
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	glAttachShader(m_RendererID, vs);
	glAttachShader(m_RendererID, fs);
	glLinkProgram(m_RendererID);

	// Check for linking errors
	int success;
	char infoLog[512];
	glGetProgramiv(m_RendererID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(m_RendererID, 512, nullptr, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	glValidateProgram(m_RendererID);

	// Check for validation errors
	glGetProgramiv(m_RendererID, GL_VALIDATE_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(m_RendererID, 512, nullptr, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::VALIDATION_FAILED\n" << infoLog << std::endl;
	}

	glDeleteShader(vs);
	glDeleteShader(fs);

	return m_RendererID;
}
