#pragma once
#ifndef ANIMATION_H
#define ANIMATION_H

#include <string>
#include <vector>
#include <map>
#include "KeyFrame.h"
#include "BoneMesh.h"
#include <assimp\scene.h>
using std::string;
using std::vector;
using std::map;
using std::pair;

struct Bone {
	string name;
	Bone* parent;
	vector<Bone*> children;
	mat4 transformation;
};

struct AnimData {
	double ticksPerSecond = 0;
	int numChannels = 0;
	double duration = 0;
	vector<KeyFrame*> keyFrames;
	string name;
	
	~AnimData() {
		for (KeyFrame* keyFrame : keyFrames) {
			delete keyFrame;
		}
		keyFrames.clear();
	}
};


struct BoneInfo {
	mat4 BoneOffset;
	int boneId;
};

class Animation {
	//used int initialization
	bool weaponProcessed = false;
	//
	string fileName;
	string name;
	bool loaded = false;
	string error;
	//bone attributes
	const int MAX_BONES = 4;
	map<string, int> boneIds;
	vector<BoneInfo> boneInfo;
	map<string, mat4>* modelTransformations;
	mat4 globalInvTransMatrix;
	int lastBoneId = 0;
	Bone* rootBone = NULL;
	//animation attributes
	AnimData* animData;
	map<int, mat4>* boneTransformations;
	float currentFrameTime = 0;
	vector<BoneMesh*> meshes;
	bool needDraw = true;
	

	Animation();
	Animation(const Animation& anim);

	void init();
	void updatePose(map<int, mat4> currentPose, Bone* bone, mat4 parentTransform);
	KeyFrame** Animation::getNextAndPrevFrames(double animTime);
	bool isBone(aiAnimation* animation, string nodeName);
	void createSkeleton(const aiScene* scene, aiNode* node, Bone* upgrNode);
	void extractBoneInformation(const aiScene* scene, aiNode* node);
	void initBoneTransform(aiNode* node);
	void showSkeleton(Bone* bone);
	void removeSkeleton(Bone* bone);
public:
	~Animation();

	//TODO testing weapon - TMP
	Bone* weaponBone;
	mat4 weaponOffset;
	BoneMesh* weaponMesh = NULL;
	map <double, BoneTransform > weaponPoses;
	mat4 weaponFinalTransformation = mat4(1.0f);
	map<string,Bone*> armParts;

	Animation(string fileName, string name, vector<BoneMesh*>& meshes, BoneMesh*& weaponMesh);
	
	map<int,mat4>* getMatrices();
	string getName();
	bool isLoaded();
	bool update(float elapsedTime, bool loop);
	void clearChanges();
};
#endif