#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <vector>

#include "include/GL/glew.h"
#include "glm/glm.hpp"
#include"RenderObject.h"

class FileManager
{
public:
	static FileManager* instance() {

		static FileManager instance;

		return &instance;
	}
public : 
	GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path);
	void LoadOBJ(
		const char* path,
		std::vector<glm::vec3>& out_vertices,
		std::vector<glm::vec2>& out_uvs,
		std::vector<glm::vec3>& out_normals);
	GLuint LoadDDS(const char* imagepath);

public:
	void loadObj(RenderObject* _object, const char* objname,
		const char* texturename, const char* vs_shader,
		const char* fs_shader);
private : 
	GLuint VertexShaderID;
	GLuint FragmentShaderID;	
};

#endif // !FILEMANAGER_H
