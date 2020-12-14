#pragma once
#ifndef SIMPLEMODEL_H
#define SIMPLEMODEL_H
#pragma comment(lib, "glew32s.lib")

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define STB_IMAGE_IMPLEMENTATION
//#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "mesh.h"
#include "shader.h"
#include <string>
#include <vector>
#include <gl/GL.h>
using namespace std;
using namespace Assimp;
using namespace glm;

class SimpleModel
{
private:
	/*  Model Data */
	glm::mat4 matrix;
	float width = 0, height = 0, depth = 0;
	float x = 1000, y = 1000, z = 1000;
	string name;
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	bool hasTexture;
	GLuint vao, vbo, ibo;
	unsigned int textureId=-1;

	void SimpleModel::initModel(string texturePath);
public:
	SimpleModel(vector<Vertex> vertices, vector<unsigned int> indices, string texturePath = "");
	~SimpleModel();
	vector<Vertex> getVertices();
	vector<unsigned int> getIndices();
	void setName(string n);
	void draw(Shader& shader, GLenum drawMode);
	void translate(float x, float y, float z);

	void scale(float x, float y, float z);
	glm::mat4 getMatrix();
	float getWidth();
	float getHeight();
	float getDepth();
	glm::vec3 getVolume();
	glm::vec3 getCoords();
	void setCoords(glm::vec3 coord);
	void updateVertices(vector<Vertex> newVertices);
	void updateIndices(vector<unsigned int> newVertices);

};
#endif

