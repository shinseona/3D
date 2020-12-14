#pragma once
#ifndef STATICMODEL_H
#define STATICMODEL_H
#pragma comment(lib, "glew32s.lib")

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#ifndef  STBI_LOADED_
#define STBI_LOADED_
#include <stb/stb_image.h>
#endif
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "mesh.h"
#include "shader.h"
#include "Properties.h"
#include "IModel.h"
#include <string>
#include <map>
#include <vector>
#include <gl/GL.h>
using namespace std;
using namespace Assimp;
using namespace glm;



class StaticModel : public IModel
{
private:
	//tmp, used in initialization
	map<string, int> textureIndexes;
	

	void processNode(aiNode *node, const aiScene *scene);
	Mesh* processMesh(aiMesh *mesh, const aiScene *scene);
	void load(string const &path);
public:
	//other
	vector<Mesh*> meshes;
	vector<mat4> meshTransformations;

	StaticModel();
	StaticModel(ModelProperty modelProperty, bool gamma = false);
	~StaticModel();

	vector<Vertex> getVertices();
	vector<unsigned int> getIndices();
	void draw(Shader*& shader);
};
#endif
