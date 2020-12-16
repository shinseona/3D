#pragma once

#include <vector>

#include "include/GL/glew.h"
#include "glm/glm.hpp"

class FileManager
{
public : 
	std::vector<GLfloat>ReadCsvData(const char* _filePath);
	GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path);
	void LoadOBJ(
		const char* path,
		std::vector<glm::vec3>& out_vertices,
		std::vector<glm::vec2>& out_uvs,
		std::vector<glm::vec3>& out_normals);
	GLuint LoadDDS(const char* imagepath);

private : 
	GLuint VertexShaderID;
	GLuint FragmentShaderID;	
};

