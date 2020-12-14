#pragma once
#ifndef _ALGRAYBOXABB_H_
#define  _ALGRAYBOXABB_H_

#include <glm/vec3.hpp>
using glm::vec3;

class AlgRayBoxAABB {
private:
	double tnear, tfar;
public:
	bool isCollided(vec3 boxMin, vec3 boxMax, vec3 rayOrigin, vec3 rayDir);
	double getTNear();
	double getTFar();
};

#endif