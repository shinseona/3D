#pragma once
#ifndef UNITMODEL_H
#define UNITMODEL_H

#include <string>
#include <vector>
#include <map>
#include <glm/vec3.hpp>
#include "BoneMesh.h"
#include "Animation.h"
#include "IModel.h"
using std::multimap;
using glm::quat;
using glm::vec3;
using glm::mat4;
using std::string;
using std::vector;
using namespace Assimp;

enum ModelPart {
	HEAD,
	ARMOR,
	BELOW,
	OTHER
};

class UnitModel : public IModel {
private:
	//tmp, used in initialization
	map<string, int> textureIndexes;
	//model attributes
	map<int, ModelPart> modelParts;//each type in this array link to meshes animations
	map<string, Animation*> animations;
	Animation* currAnimation;
	bool loop = false;
	bool needUpdate = false;
	bool running = false;
	vector<BoneMesh*> meshes;
	//testing - TMP
	BoneMesh* weaponMesh = NULL;
	mat4 weaponModelMatrix;

	UnitModel();

	//initializing
	void load(string const &path);
	void processNode(aiNode* node, const aiScene* scene);
	BoneMesh* processMesh(aiMesh* mesh, const aiScene* scene, int meshId);
public:
	//(des/con)structors
	UnitModel(const ModelProperty prop);
	UnitModel(const UnitModel& m);
	~UnitModel();

	//setters
	void setAnimation(string animName, bool loop = false);

	//=====control
	//general
	void draw(Shader& animShader);
	void update(float elapsedTime);
	void addAnimation(string name, string path);
	void addAnimation(Animation* anim);
	void goTo(float x, float y, float z);//overrided
	void translate(float x, float y, float z);//overrided
	//changing skin
	void changeTexture(ModelPart part, ModelProperty texturePath);
	//=============

	//other
	UnitModel& operator=(const UnitModel& model);
	UnitModel* operator=(const UnitModel* model);
};


#endif