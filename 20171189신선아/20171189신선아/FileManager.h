#pragma once

#include <string>
#include <vector>

#include "include/GL/glew.h"		
#include "include/GLFW/glfw3.h" 
#include "glm/gtc/matrix_transform.hpp"
#include "glm/glm.hpp"

#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib, "lib-vc2017/glew32.lib")
#pragma comment(lib, "lib-vc2017/glfw3.lib")

class RenderableObject;

class FileManager
{
public:
	static FileManager* instance()
	{
		static FileManager instance;

		return &instance;
	}

public:
	void loadObj(
		RenderableObject* obj,
		std::string obj_path,
		std::string texture_path,
		std::string vs_shader_path,
		std::string fs_shader_path
	);
	void loadObj2(
		RenderableObject* obj,
		std::string obj_path,
		std::string vs_shader_path,
		std::string fs_shader_path
	);
	bool loadOBJ(
		const char* path,
		std::vector<glm::vec3>& out_vertices,
		std::vector<glm::vec2>& out_uvs,
		std::vector<glm::vec3>& out_normals
	);
	GLuint loadBMP(const char* imagepath);
	GLuint loadShaders(const char* vertex_file_path, const char* fragment_file_path);
};