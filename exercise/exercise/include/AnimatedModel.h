#pragma once
#ifndef ANIMATEDMODEL_H
#define ANIMATEDMODEL_H

#include <string>
#include <vector>
#include <map>
#include <glm/vec3.hpp>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include "BoneMesh.h"
#include "mesh.h"
#include "Tile.h"
#include "KeyFrame.h"
#include "Animation.h"
#include "IModel.h"
using std::multimap;
using glm::quat;
using glm::vec3;
using glm::mat4;
using std::string;
using std::vector;
using namespace Assimp;

class AnimatedModel : public IModel{
private:
	//tmp, used in initialization
	map<string, int> textureIndexes;
	//other
	Animation* animation;
	bool loop=false;
	bool needUpdate =false;
	bool running = false;
	vector<BoneMesh*> meshes;

	//initializing
	void load(string const &path);
	void processNode(aiNode* node, const aiScene* scene);
	BoneMesh* processMesh(aiMesh* mesh, const aiScene* scene);
public:
	//(des/con)structors
	AnimatedModel(ModelProperty& const property);
	AnimatedModel();
	~AnimatedModel();

	//=====control
	void draw(Shader animShader);
	void update(float elapsedTime);
	//=============

	//other
	AnimatedModel& operator=(AnimatedModel& model);
	AnimatedModel* operator=(const AnimatedModel* model);
};

 
#endif