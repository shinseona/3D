#include <map>
#include <iterator>
#include "KeyFrame.h"
using std::map;
using std::iterator;

KeyFrame::KeyFrame(float timeStamp, map<int, BoneTransform> poses) {
	this->timeStamp = timeStamp;
	this->poses = poses;
}
KeyFrame::~KeyFrame() {
	poses.clear();
}

BoneTransform BoneTransform::interpolate(BoneTransform t1, BoneTransform t2, float progress) {
	BoneTransform result;
	result.position = glm::mix(t1.position, t2.position, progress);//interpolate(t1.position, t2.position, progress);
	result.rotation = glm::slerp(t1.rotation, t2.rotation, progress);
	result.scaling = glm::mix(t1.scaling, t2.scaling, progress);
	return result;
}
mat4 BoneTransform::toMatrix() {
	mat4 matrix(1.0f);
	mat4 rot = mat4_cast(rotation);
	mat4 scal = glm::scale(mat4(1.0), scaling);
	mat4 pos = glm::translate(mat4(1.0), position);

	return pos*rot*scal;
	//return scal*rot*pos;
}
vec3 BoneTransform::interpolate(vec3 v1, vec3 v2, float progress) {
	return v1 + progress*(v2 - v1);
}
quat BoneTransform::interpolate(quat v1, quat v2, float progress) {
	return  lerp(v1, v2, progress);
}

float KeyFrame::getTimeStamp() {
	return timeStamp;
}
map<int, BoneTransform> KeyFrame::getPoses() {
	return poses;
}

BoneTransform* KeyFrame::getPose(int boneId) {
	if (poses.find(boneId) != poses.end()) {
		return &poses[boneId];
	}

	return NULL;
}