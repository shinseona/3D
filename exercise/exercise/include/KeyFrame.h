#pragma once
#ifndef KEYFRAME_H
#define KEYFRAME_H

#include <string>
#include <map>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
using std::string;
using std::map;
using glm::quat;
using glm::vec3;
using glm::mat4;

struct BoneTransform {
	vec3 position;
	quat rotation;
	vec3 scaling;
	double posTime;
	double rotTime;
	double scalTime;

	static BoneTransform interpolate(BoneTransform t1, BoneTransform t2, float progress);
	mat4 toMatrix();
private:
	static vec3 interpolate(vec3 v1, vec3 v2, float progress);
	static quat interpolate(quat v1, quat v2, float progress);
};

class KeyFrame {
	/*
	Transformation of each bone per frame
	POSES
	Key(bone name)  Value(rot,scal and pos)
	Leg  (2.3,2.1,5.6)(45,56,12)
	Arm  (6.1,1.7,7.2)(89,56,42)
	Head  (10.3,1.1,1.8)(11,34,93)
	....
	*/
	map<int, BoneTransform> poses;//current poses of bones
	float timeStamp;//time when frame starts
public:
	KeyFrame(float timeStamp, map<int, BoneTransform> poses);
	~KeyFrame();

	float getTimeStamp();
	map<int, BoneTransform> getPoses();
	BoneTransform* getPose(int boneId);
};
#endif