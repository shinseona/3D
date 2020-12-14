#include "Animation.h"
#include <iostream>
#include <algorithm>
#include <iterator>
#include <sstream>
#include <glm/gtx/matrix_operation.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Helper.h"
using std::cout;
using std::endl;
using namespace glm;
using namespace Assimp;

Animation::Animation(string fileName, string name, vector<BoneMesh*>& meshes, BoneMesh*& weaponMesh) {
	cout << "Animation::Animation(f,n,m,w)1\n";
	this->weaponMesh = weaponMesh;
	cout << "Animation::here2\n";
	cout << "Meshes.size: " << meshes.size() << endl;
	cout << "Meshes[0].nrVertices: " << meshes.at(0)->getVertices().size() << endl;
	for(int a=0; a < meshes.size(); ++a)
		this->meshes.push_back(meshes.at(a));
	cout << "Animation::here3\n";
	this->name = name;
	this->fileName = fileName;

	init();
	cout << "Animation::Animation(f,n,m,w)2\n";
}
Animation::Animation() {}
Animation::Animation(const Animation& anim) {}
Animation::~Animation() {
	cout << "Animation::~Animation1" << endl;
	boneIds.clear();
	boneInfo.clear();
	modelTransformations->clear();
	if (rootBone != NULL)
		removeSkeleton(rootBone);
	cout << "Animation::~Animation2" << endl;
	if (animData != NULL)
		delete animData;
	cout << "Animation::~Animation3" << endl;
	if (boneTransformations != NULL) {
		boneTransformations->clear();
		delete boneTransformations;
	}
	cout << "Animation::~Animation4" << endl;
	
	cout << "Animation::~Animation5" << endl;
}

map<int, mat4>* Animation::getMatrices() {
	return boneTransformations;
}

void Animation::updatePose(map<int, mat4> currentPose, Bone* bone, mat4 parentTransform) {
	int boneId = boneIds.at(bone->name);
	mat4 currLocalTransform = currentPose.at(boneId);
	mat4 globalTransform = parentTransform*currLocalTransform;
	boneTransformations->at(boneId) = globalInvTransMatrix *globalTransform * boneInfo.at(boneId).BoneOffset;

	for (Bone* childBone : bone->children) {
		updatePose(currentPose, childBone, globalTransform);
	}

}


void Animation::showSkeleton(Bone* bone) {
	cout << "Bone: " << bone->name << endl;
	for (Bone* b : bone->children) {
		showSkeleton(b);
	}
}
void Animation::removeSkeleton(Bone* bone) {
	for (Bone* b : bone->children) {
		removeSkeleton(b);
	}
	delete bone;
}

void Animation::init() {
	cout << "Animation: Loading model '" << fileName << "'..." << endl;
	loaded = true;
	Importer importer;
	unsigned int flags = aiProcess_Triangulate |
	aiProcess_FlipUVs |
	aiProcess_CalcTangentSpace | aiProcess_GenNormals | aiProcess_LimitBoneWeights;
	const aiScene* scene = importer.ReadFile(fileName, flags);
	if (!scene) {
		cout << "AnimatedModel::Error::(" << fileName << "): Null scene after reading the file!" << endl;
		loaded = false;
	}
	else if (scene->mFlags&& AI_SCENE_FLAGS_INCOMPLETE) {
		cout << "AnimatedModel::Error::(" << fileName << "): Incomplete flags!" << endl;
		loaded = false;
	}
	else if (!scene->mRootNode) {
		cout << "AnimatedModel::Error::(" << fileName << "): Null root node!" << endl;
		loaded = false;
	}
	if (!loaded) return;

	//init
	Helper h;
	modelTransformations = new map<string, mat4>();
	boneTransformations = new map<int, mat4>();
	globalInvTransMatrix = inverse(h.aiMatrix4x4ToGlm(scene->mRootNode->mTransformation));

	//---extract bone information
	if (scene->mAnimations != NULL) {
		if (scene->mAnimations[0]->mChannels != NULL) {
			//create bone hierarchy
			createSkeleton(scene, scene->mRootNode, rootBone);
			cout << "Skeleton created\n";

			//extract vertex weights and bone indices
			extractBoneInformation(scene, scene->mRootNode);
			cout << "Bone information extracted\n";

			//insert bone offsets for bones that dont change model(I believe this is temporal solution...)
			vector<int> allIds;
			vector<int> infoIds;
			vector<int> differenceIds;
			for (pair<string, int> boneId : boneIds) {
				allIds.push_back(boneId.second);
			}
			for (BoneInfo info : boneInfo) {
				infoIds.push_back(info.boneId);
			}

			//sort for difference algorithm
			sort(allIds.begin(), allIds.end());
			sort(infoIds.begin(), infoIds.end());

			//difference algorithm: search all bone IDs that don't change model 
			std::set_difference(allIds.begin(),
				allIds.end(),
				infoIds.begin(),
				infoIds.end(),
				std::back_inserter(differenceIds));

			//add all bone offset for absent bones
			for (int a = 0; a < differenceIds.size(); ++a) {
				BoneInfo info;
				info.boneId = differenceIds.at(a);
				info.BoneOffset = mat4(1.0f);
				boneInfo.push_back(info);
			}
		}
		else { error = "Bones not found"; loaded = false; }
	}
	else { error = "Animations not found";  loaded = false; }

	if (loaded) {
		cout << "Animation: Bone information extracted, skeleton formed\n";
		for(int a=0; a < meshes.size(); ++a)
			if(meshes.at(a)!=NULL)
				meshes.at(a)->update();//notify mesh that vertices were update(added bone IDs and weights)
		if(weaponMesh!=NULL)
			weaponMesh->update();
	}
	else {
		cout << "Animation::Error: "<<error<<"\n";
		return;
	}
	//------------------

	//---extract animation information
	if (scene->mAnimations != NULL) {
		animData = new AnimData();
		aiAnimation** animations = scene->mAnimations;
		aiAnimation* animation = animations[0];
		animData->ticksPerSecond = animation->mTicksPerSecond;
		animData->numChannels = animation->mNumChannels;
		animData->duration = animation->mDuration;
		animData->name = animation->mName.C_Str();

		vector<KeyFrame*> keyFrames;
		KeyFrame* keyFrame;
		if (animation->mNumChannels > 0) {
			aiNodeAnim** channels = animation->mChannels;
			int nrKeyFrames = channels[0]->mNumPositionKeys;
			for (int nrKeyFrame = 0; nrKeyFrame < nrKeyFrames; ++nrKeyFrame) {
				aiNodeAnim* channel = channels[0];
				double keyTime = channel->mRotationKeys[nrKeyFrame].mTime;
				cout << "KeyTime: " << keyTime << endl;
				map<int, BoneTransform> poses;
				for (int nrChannel = 0; nrChannel < animation->mNumChannels; ++nrChannel) {
					channel = channels[nrChannel];
					string channelName(channel->mNodeName.C_Str());

					cout << "ChannelName: " << channelName << endl;
				//	if (channelName == "Weapon") continue;

					quat rotKey = h.assimpQuatToGlmQuat(channel->mRotationKeys[nrKeyFrame].mValue);
					vec3 scalKey = h.assimpVec3ToGlmVec3(channel->mScalingKeys[nrKeyFrame].mValue);
					vec3 posKey = h.assimpVec3ToGlmVec3(channel->mPositionKeys[nrKeyFrame].mValue);
					BoneTransform transform;
					transform.rotation = rotKey;
					transform.scaling = scalKey;
					transform.position = posKey;

					if (channelName == "Weapon") {
						weaponPoses.insert(make_pair(keyTime, transform));
					} else if (boneIds.find(channelName) != boneIds.end()) {
						int boneId = boneIds.at(channelName);
						poses.insert(make_pair(boneId, transform));
					}

					
				}
				keyFrame = new KeyFrame(keyTime, poses);
				keyFrames.push_back(keyFrame);
			}
			animData->keyFrames = keyFrames;
		}
		else { loaded = false; error = "Channels not found";}
	}
	else { loaded = false; error = "Animations not found";}

	if (loaded)
		cout << "Animation: Animation information added." << endl;
	else {
		cout << "Animation::Error: "<< error <<endl;
		return;
	}

	//init bone transformation
	initBoneTransform(scene->mRootNode);
}

bool Animation::update(float elapsedTime, bool loop) {
	if (!loaded) { cout << "Animation of '" << fileName << "' not loaded!\n";  return true; }
	Helper h;

	/*//calculate time
	currentFrameTime += elapsedTime;
	float timeInSeconds = currentFrameTime / 1000.0f;
	if (timeInSeconds >= animData->duration) {
	currentFrameTime = fmod(currentFrameTime, animData->duration*1000.0f);
	}
	float ticksPerSecond = animData->ticksPerSecond != 0 ? animData->ticksPerSecond : 25.0f;
	float timeInTicks = ticksPerSecond*timeInSeconds;
	float animationTime = animData->duration == 0 ? 0.0f : (float)fmod(timeInTicks, animData->duration);*/

	elapsedTime *= 0.002;
	currentFrameTime += elapsedTime;
	if (currentFrameTime >= animData->duration) {
		if (!loop) {
			currentFrameTime = 0.0;
		//	clearChanges();
			return true;
		}
		currentFrameTime = fmod(currentFrameTime, animData->duration);
	}

	/*cout << "TimeInSeconds: " << timeInSeconds << endl;
	cout << "ticksPerSecond: " << ticksPerSecond << endl;
	cout << "timeInTicks: " << timeInTicks << endl;
	cout << "animationTime: " << animationTime << endl;
	cout << "duration: " <<  animData.duration << endl;
	cout << "---------------------" << endl;*/

	//get frames that are bounds of current time
	KeyFrame** frames = getNextAndPrevFrames(currentFrameTime);
	KeyFrame* prevFrame = frames[0];
	KeyFrame* nextFrame = frames[1];
	float totalTime = nextFrame->getTimeStamp() - prevFrame->getTimeStamp();
	float currentTime = currentFrameTime - prevFrame->getTimeStamp();
	float progress = currentTime / totalTime;

	//calculate current weapon transformation and update
	pair<double, BoneTransform> lastPair;
	for (pair<double, BoneTransform> t : weaponPoses) {
		if (t.first > currentFrameTime) {
			BoneTransform currentTransform = BoneTransform::interpolate(lastPair.second, t.second, progress);
			mat4 currLocalTransform = currentTransform.toMatrix();
			mat4 parentTransform = mat4(1.0f);
			mat4 globalTransform = parentTransform*currLocalTransform;
			weaponFinalTransformation = globalInvTransMatrix * globalTransform * weaponOffset;

			break;
		}
		lastPair = t;
	}

	//calculate current pose
	map<int, mat4> currentPose;
	for (auto iterBoneId = boneIds.begin(); iterBoneId != boneIds.end(); ++iterBoneId) {
		int boneId = ((pair<string, int>)*iterBoneId).second;
		BoneTransform prevTransform = prevFrame->getPoses().at(boneId);
		BoneTransform nextTransform = nextFrame->getPoses().at(boneId);
		BoneTransform currentTransform = BoneTransform::interpolate(prevTransform, nextTransform, progress);
		mat4 currentTransformationMatrix = currentTransform.toMatrix();
		currentPose.insert(make_pair(boneId, currentTransformationMatrix));
	}

	//update current pose
	updatePose(currentPose, rootBone, mat4(1.0f));
	
	if(false)
	if (weaponBone != NULL) {
		//calculate arm1,arm2,arm3 positions
		//arm1 position
		int boneId = boneIds.at("arm1");
		mat4 currLocalTransform = currentPose.at(boneId);
		mat4 parentTransform = weaponFinalTransformation;
		mat4 globalTransform = parentTransform*currLocalTransform;
		boneTransformations->at(boneId) = globalInvTransMatrix*globalTransform*boneInfo.at(boneId).BoneOffset;
		////arm2 position
		//boneId = boneIds.at("arm2");+//-
		//currLocalTransform = currentPose.at(boneId);
		//parentTransform = globalTransform;
		//globalTransform = parentTransform*currLocalTransform;
		//currentPose.at(boneId) = globalInvTransMatrix * globalTransform * boneInfo.at(boneId).BoneOffset;
		////arm3 position
		//boneId = boneIds.at("arm3");
		//currLocalTransform = currentPose.at(boneId);
		//parentTransform = globalTransform;
		//globalTransform = parentTransform*currLocalTransform;
		//currentPose.at(boneId) = globalInvTransMatrix * globalTransform * boneInfo.at(boneId).BoneOffset;
	}

	

	/*printf("PrevTimeStamp: %.5f\n\
	NextTimeStamp: %.5f\n\
	AnimDuration: %.5f\n\
	ElapsedTime: %.5f\n\
	TotalTime: %.5f\n\
	CurrentTime: %.5f \n\
	Progress: %.5f\n", prevFrame->getTimeStamp(), nextFrame->getTimeStamp(),
	animData->duration, elapsedTime, totalTime, currentTime, progress);
	cout << "-------------\n";*/

	return false;
}

KeyFrame** Animation::getNextAndPrevFrames(double animTime) {
	vector<KeyFrame*> frames = animData->keyFrames;
	KeyFrame* prevFrame = frames[0];
	KeyFrame* nextFrame = frames[0];
	for (int nrFrame = 1; nrFrame < frames.size(); ++nrFrame) {
		nextFrame = frames[nrFrame];
		if (nextFrame->getTimeStamp() > animTime)
			break;
		prevFrame = nextFrame;
	}
	KeyFrame** prevAndNext = new KeyFrame*[2];
	prevAndNext[0] = prevFrame;
	prevAndNext[1] = nextFrame;
	return prevAndNext;
}


bool Animation::isBone(aiAnimation* animation, string nodeName) {
	aiNodeAnim** channels = animation->mChannels;
	unsigned int nrChannels = animation->mNumChannels;
	bool found = false;
	for (int a = 0; a < nrChannels; ++a) {
		string name(channels[a]->mNodeName.C_Str());
		if (name.compare(nodeName) == 0) {
			found = true;
			break;
		}
	}
	return found;
}
void Animation::createSkeleton(const aiScene* scene, aiNode* node, Bone* upgrNode) {

	Helper h;
	string nodeName = node->mName.C_Str();
	mat4 nodeTransformation = h.aiMatrix4x4ToGlm(node->mTransformation);
	aiNode* parent = node->mParent;
	string parentName = parent == NULL ? "NO ROOT" : parent->mName.C_Str();

	//check if node is bone
	bool nodeIsBone = isBone(scene->mAnimations[0], nodeName);

	if (nodeIsBone){
		if(nodeName == "Weapon"){//if this is weapon bone
			weaponBone = new Bone();
			weaponBone->transformation = nodeTransformation;
			weaponBone->name = nodeName;
			weaponBone->parent = NULL;
			cout << "SkeletalBone(weapon): " << nodeName << endl;
			return;
		} else {

			//insert ID map to NAME
			boneIds.insert(make_pair(nodeName, lastBoneId++));
			cout << "SkeletalBone: " << nodeName << endl;
		}
	}
	

	//check if is root bone
	if (nodeIsBone && upgrNode == NULL) {
		Bone* newRoot = new Bone();
		newRoot->transformation = nodeTransformation;
		newRoot->name = nodeName;
		newRoot->parent = NULL;
		rootBone = upgrNode = newRoot;
	}


	if (nodeName == "arm1" || nodeName == "arm1" || nodeName == "arm1") {
		Bone* armBone = new Bone();
		armBone->name = nodeName;
		armBone->transformation = nodeTransformation;
		armParts.insert(make_pair(nodeName, armBone));
	}

	//for each child do the same operations
	for (int nrChild = 0; nrChild < node->mNumChildren; ++nrChild) {
		if (nodeIsBone) {
			aiNode* child = node->mChildren[nrChild];
			Bone* upgrChild = new Bone();
			upgrChild->transformation = h.aiMatrix4x4ToGlm(child->mTransformation);
			upgrChild->name = child->mName.C_Str();
			upgrChild->parent = upgrNode;
			upgrNode->children.push_back(upgrChild);
			createSkeleton(scene, node->mChildren[nrChild], upgrChild);
		}
		else
			createSkeleton(scene, node->mChildren[nrChild], upgrNode);
	}
}
void Animation::extractBoneInformation(const aiScene* scene, aiNode* node) {
	Helper h;
	string nodeName = node->mName.C_Str();
	mat4 nodeTransformation = h.aiMatrix4x4ToGlm(node->mTransformation);
	aiNode* parent = node->mParent;
	string parentName = parent == NULL ? "NO_PARENT" : parent->mName.C_Str();

	//cout << "--Node '"+nodeName+"' Transformation--\n";
	//h.showMatrix(nodeTransformation);
	cout << "ExtrBoneInfo::NodeName: " << nodeName << ", Parent: " << parentName << endl;

	//iterate all meshes to save bone information and vertex weights
	unsigned int* meshesIndexes = node->mMeshes;
	int nrMeshes = node->mNumMeshes;
	for (int nrMesh = 0; nrMesh < nrMeshes; ++nrMesh) {
		int meshId = meshesIndexes[nrMesh];
		aiMesh* mesh = scene->mMeshes[meshId];
		string meshName(mesh->mName.C_Str());

		aiBone** bones = mesh->mBones;
		if (bones != NULL) {
			int nrBones = mesh->mNumBones;
			cout << "Nr. Bones: " << nrBones << endl;
			for (int nrBone = 0; nrBone < nrBones; ++nrBone) {
				aiBone* bone = bones[nrBone];
				string boneName = string(bone->mName.C_Str());
				mat4 boneOffset = h.aiMatrix4x4ToGlm(bone->mOffsetMatrix);
				if (boneName == "Weapon") {
					if (weaponMesh == NULL) continue;
					cout << "Processing bones... BoneName[weapon]: " << boneName << endl;
					this->weaponOffset = boneOffset;
					int nrWeights = bone->mNumWeights;
					for (int nrWeight = 0; nrWeight < nrWeights; ++nrWeight) {
						aiVertexWeight vertexWeight = bone->mWeights[nrWeight];
						float weight = vertexWeight.mWeight;
						uint vertexId = vertexWeight.mVertexId;
						weaponMesh->addBoneData(vertexId, weight, 0);
					}
					weaponProcessed = true;
				}
				else {
					unsigned int boneId = boneIds.at(boneName);
					cout << "Processing bones... BoneName: " << boneName << endl;
					//mapping bone name to bone id
					BoneInfo info;
					info.BoneOffset = boneOffset;
					info.boneId = boneId;
					//save bone offset in boneInfo, if isn't here
					bool found = false;
					for (BoneInfo b : boneInfo) {
						if (b.boneId == boneId) {
							found = true;
						}
					}
					if (!found)
						boneInfo.push_back(info);
					
					//add vertex weights and bone ids to mesh
					int nrWeights = bone->mNumWeights;
					for (int nrWeight = 0; nrWeight < nrWeights; ++nrWeight) {
						aiVertexWeight vertexWeight = bone->mWeights[nrWeight];
						float weight = vertexWeight.mWeight;
						uint vertexId = vertexWeight.mVertexId;

						if(weaponProcessed)
							meshes.at(meshId-1)->addBoneData(vertexId, weight, boneId);
						else
							meshes.at(meshId)->addBoneData(vertexId, weight, boneId);
					}
				}
			}
		}
	}

	//for each child do the same operations
	for (int nrChild = 0; nrChild < node->mNumChildren; ++nrChild) {
		extractBoneInformation(scene, node->mChildren[nrChild]);
	}
}
void Animation::initBoneTransform(aiNode* node) {
	Helper h;
	string currNodeName(node->mName.C_Str());
	cout << currNodeName << endl;
	if (boneIds.find(currNodeName) != boneIds.end()) {
		int nodeId = boneIds.at(currNodeName);
		mat4 transformation = h.aiMatrix4x4ToGlm(node->mTransformation);
		boneTransformations->insert(make_pair(nodeId, transformation));
	}
	for (int nrChild = 0; nrChild < node->mNumChildren; ++nrChild) {
		initBoneTransform(node->mChildren[nrChild]);
	}
}

bool Animation::isLoaded() { return loaded; }
string Animation::getName() { return name; }

void Animation::clearChanges() {
	/*for (auto it = boneTransformations->begin(); it != boneTransformations->end(); ++it)
	{
		(*it).second = globalInvTransMatrix * (*it).second;
	}*/

	//get frames that are bounds of current time
	double time = 0.0;
	KeyFrame** frames = getNextAndPrevFrames(time);
	KeyFrame* prevFrame = frames[0];
	KeyFrame* nextFrame = frames[1];
	float totalTime = nextFrame->getTimeStamp() - prevFrame->getTimeStamp();
	float currentTime = currentFrameTime - prevFrame->getTimeStamp();
	float progress = currentTime / totalTime;

	//calculate current weapon transformation and update
	pair<double, BoneTransform> lastPair;
	for (pair<double, BoneTransform> t : weaponPoses) {
		if (t.first > currentFrameTime) {
			BoneTransform currentTransform = BoneTransform::interpolate(lastPair.second, t.second, progress);
			mat4 currLocalTransform = currentTransform.toMatrix();
			mat4 parentTransform = mat4(1.0f);
			mat4 globalTransform = parentTransform*currLocalTransform;
			weaponFinalTransformation = globalInvTransMatrix * globalTransform * weaponOffset;

			break;
		}
		lastPair = t;
	}


	

	//calculate current pose
	map<int, mat4> currentPose;
	for (auto iterBoneId = boneIds.begin(); iterBoneId != boneIds.end(); ++iterBoneId) {
		int boneId = ((pair<string, int>)*iterBoneId).second;
		BoneTransform prevTransform = prevFrame->getPoses().at(boneId);
		BoneTransform nextTransform = nextFrame->getPoses().at(boneId);
		BoneTransform currentTransform = BoneTransform::interpolate(prevTransform, nextTransform, progress);
		mat4 currentTransformationMatrix = currentTransform.toMatrix();
		currentPose.insert(make_pair(boneId, currentTransformationMatrix));
	}

	//update current pose
	updatePose(currentPose, rootBone, mat4(1.0f));

	
}
