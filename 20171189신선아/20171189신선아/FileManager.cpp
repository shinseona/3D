#define _CRT_SECURE_NO_WARNINGS

#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib, "lib-vc2017/glew32.lib")
#pragma comment(lib, "lib-vc2017/glfw3.lib")
#pragma comment(lib, "winmm.lib")

#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>

#include "Object.h"
#include "FileManager.h"
#include "RenderableObject.h"
#include "Human.h"
#include "include/GL/glew.h"

#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII

void FileManager::loadObj(RenderableObject* obj,
	std::string obj_path,
	std::string texture_path,
	std::string vs_shader_path,
	std::string fs_shader_path)
{
	obj->programID = loadShaders(vs_shader_path.c_str(), fs_shader_path.c_str());
	obj->MatrixID = glGetUniformLocation(obj->programID, "MVP");
	obj->Texture = loadBMP(texture_path.c_str());
	obj->TextureID = glGetUniformLocation(obj->programID, "myTextureSampler");

	glGenVertexArrays(1, &obj->VertexArrayID);
	glBindVertexArray(obj->VertexArrayID);

	bool res = loadOBJ(obj_path.c_str(), obj->vertices, obj->uvs, obj->normals);

	glGenBuffers(1, &obj->vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, obj->vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, obj->vertices.size() * sizeof(glm::vec3), &obj->vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &obj->uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, obj->uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, obj->uvs.size() * sizeof(glm::vec2), &obj->uvs[0], GL_STATIC_DRAW);

	glGenBuffers(1, &obj->normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, obj->normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, obj->normals.size() * sizeof(glm::vec3), &obj->normals[0], GL_STATIC_DRAW);
}

void FileManager::loadObj2(RenderableObject* obj,
	std::string obj_path,
	std::string vs_shader_path,
	std::string fs_shader_path)
{
	obj->programID = loadShaders(vs_shader_path.c_str(), fs_shader_path.c_str());
	obj->MatrixID = glGetUniformLocation(obj->programID, "MVP");
	obj->TextureID = glGetUniformLocation(obj->programID, "myTextureSampler");

	glGenVertexArrays(1, &obj->VertexArrayID);
	glBindVertexArray(obj->VertexArrayID);

	bool res = loadOBJ(obj_path.c_str(), obj->vertices, obj->uvs, obj->normals);

	glGenBuffers(1, &obj->vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, obj->vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, obj->vertices.size() * sizeof(glm::vec3), &obj->vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &obj->uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, obj->uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, obj->uvs.size() * sizeof(glm::vec2), &obj->uvs[0], GL_STATIC_DRAW);

	glGenBuffers(1, &obj->normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, obj->normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, obj->normals.size() * sizeof(glm::vec3), &obj->normals[0], GL_STATIC_DRAW);
}

bool FileManager::loadOBJ(
	const char* path,
	std::vector<glm::vec3>& out_vertices,
	std::vector<glm::vec2>& out_uvs,
	std::vector<glm::vec3>& out_normals
) {
	printf("Loading OBJ file %s...\n", path);

	std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	std::vector<glm::vec3> temp_vertices;
	std::vector<glm::vec2> temp_uvs;
	std::vector<glm::vec3> temp_normals;


	FILE* file = fopen(path, "r");
	if (file == NULL) {
		printf("Impossible to open the file ! Are you in the right path ? See Tutorial 1 for details\n");
		getchar();
		return false;
	}

	while (1) {

		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

		// else : parse lineHeader

		if (strcmp(lineHeader, "v") == 0) {
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			temp_vertices.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0) {
			glm::vec2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y);
			uv.y = -uv.y; // Invert V coordinate since we will only use DDS texture, which are inverted. Remove if you want to use TGA or BMP loaders.
			temp_uvs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0) {
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			temp_normals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0) {
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
			if (matches != 9) {
				printf("File can't be read by our simple parser :-( Try exporting with other options\n");
				fclose(file);
				return false;
			}
			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			uvIndices.push_back(uvIndex[0]);
			uvIndices.push_back(uvIndex[1]);
			uvIndices.push_back(uvIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);
		}
		else {
			// Probably a comment, eat up the rest of the line
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}

	}

	// For each vertex of each triangle
	for (unsigned int i = 0; i < vertexIndices.size(); i++) {

		// Get the indices of its attributes
		unsigned int vertexIndex = vertexIndices[i];
		unsigned int uvIndex = uvIndices[i];
		unsigned int normalIndex = normalIndices[i];

		// Get the attributes thanks to the index
		glm::vec3 vertex = temp_vertices[vertexIndex - 1];
		glm::vec2 uv = temp_uvs[uvIndex - 1];
		glm::vec3 normal = temp_normals[normalIndex - 1];

		// Put the attributes in buffers
		out_vertices.push_back(vertex);
		out_uvs.push_back(uv);
		out_normals.push_back(normal);

	}
	fclose(file);
	return true;
}

GLuint FileManager::loadBMP(const char* imagepath)
{
	printf("Reading image %s\n", imagepath);

	//BMP파일의 헤더에서 데이터를 읽는다
	unsigned char header[54];
	unsigned int dataPos;
	unsigned int imageSize;
	unsigned int width, height;
	//실제 RGB 데이터
	unsigned char* data;

	//파일을 연다
	FILE* file = fopen(imagepath, "rb");
	if (!file) {
		printf("%s Can not Open. Check the path again.\n", imagepath);
		getchar();
		return 0;
	}

	//헤더를 읽는다, i.e. the 54 first bytes

	//만약 54 bytes보다 적게 읽혔으면 문제 발생
	if (fread(header, 1, 54, file) != 54) {
		printf("It is not BMP File\n");
		return 0;
	}
	//A BMP 파일은 항상 "BM"으로 시작한다.
	if (header[0] != 'B' || header[1] != 'M') {
		printf("It is not BMP File\n");
		return 0;
	}
	//24pp file임을 확인한다.
	if (*(int*)&(header[0x1e]) != 0 || *(int*)&(header[0x1C]) != 24) {
		printf("It is not BMP File\n");
		return 0;
	}

	//이미지에 대한 정보를 읽는다.
	dataPos = *(int*)&(header[0x0A]);
	imageSize = *(int*)&(header[0x22]);
	width = *(int*)&(header[0x12]);
	height = *(int*)&(header[0x16]);

	//몇몇 BMP 파일들은 포맷이 놓쳐졌다, 놓쳐진 정보를 추측해라
	if (imageSize == 0) imageSize = width * height * 3; // 3 : one byte for each Red-Green-Blue component
	if (dataPos == 0) dataPos = 54; //BMP 헤더는 항상 이 형식

	//버퍼를 생성한다
	data = new unsigned char[imageSize];

	//파일의 버퍼에 있는 실제 데이터를 읽는다
	fread(data, 1, imageSize, file);

	//모든 것은 현재 메모리에 있다, 파일을 닫는다
	fclose(file);

	//openGL 텍스처를 만든다
	GLuint textureID;
	glGenTextures(1, &textureID);

	//새로이 만들어진 텍스처를 바인딩한다.
	glBindTexture(GL_TEXTURE_2D, textureID);

	//이미지를 OpenGL에게 넘긴다
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

	delete[] data;

	// trilinear(삼선형) 필터링
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);

	return textureID;
}

GLuint FileManager::loadShaders(const char* vertex_file_path, const char* fragment_file_path)
{

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if (VertexShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << VertexShaderStream.rdbuf();
		VertexShaderCode = sstr.str();
		VertexShaderStream.close();
	}
	else {
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
		getchar();
		return 0;
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if (FragmentShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << FragmentShaderStream.rdbuf();
		FragmentShaderCode = sstr.str();
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;


	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const* VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}



	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const* FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}



	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}


	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}