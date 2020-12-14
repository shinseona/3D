#pragma once
#ifndef SHADER_H
#define SHADER_H
//#pragma comment(lib, "glew32s.lib")
#include <GLEW/glew.h>
#include <gl/GL.h>
#include <glm/glm.hpp>
#include <string>

enum ShaderComplexity {
	SHADERCOMPLEXITY_SIMPLE,//only position layout
	SHADERCOMPLEXITY_FULL//texture, positions, normals
};

class Shader
{
private:
	unsigned int ID;
	ShaderComplexity complexity;
	unsigned int vertexShader, fragmentShader, geometryShader;
public:
	// constructor generates the shader on the fly
	// ------------------------------------------------------------------------
	Shader(const char* vertexPath, const char* fragmentPath, ShaderComplexity complexity, const char* geometryPath = nullptr);
	~Shader();
	// activate the shader
	// ------------------------------------------------------------------------
	void use();

	Shader& get();

	//getters
	int getId();
	ShaderComplexity Shader::getComplexity();

	// utility uniform functions
	// ------------------------------------------------------------------------
	void setBool(const std::string &name, bool value) const;
	// ------------------------------------------------------------------------
	void setInt(const std::string &name, int value) const;
	// ------------------------------------------------------------------------
	void setFloat(const std::string &name, float value) const;
	// ------------------------------------------------------------------------
	void setVec2(const std::string &name, const glm::vec2 &value) const;
	void setVec2(const std::string &name, float x, float y) const;
	// ------------------------------------------------------------------------
	void setVec3(const std::string &name, const glm::vec3 &value) const;
	void setVec3(const std::string &name, float x, float y, float z) const;
	// ------------------------------------------------------------------------
	void setVec4(const std::string &name, const glm::vec4 &value) const;
	void setVec4(const std::string &name, float x, float y, float z, float w);
	// ------------------------------------------------------------------------
	void setMat2(const std::string &name, const glm::mat2 &mat) const;
	// ------------------------------------------------------------------------
	void setMat3(const std::string &name, const glm::mat3 &mat) const;
	// ------------------------------------------------------------------------
	void setMat4(const std::string &name, const glm::mat4 &mat) const;

	// utility uniform functions, by ID
	void Shader::setMat4(GLint location, const glm::mat4 &mat) const;

private:
	// utility function for checking shader compilation/linking errors.
	// ------------------------------------------------------------------------
	void checkCompileErrors(GLuint shader, std::string type);
};
#endif