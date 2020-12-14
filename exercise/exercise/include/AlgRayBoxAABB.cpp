//made by Romeon0
#include <glm/vec3.hpp>
#include <iostream>
#include "AlgRayBoxAABB.h"
using glm::vec3;

bool AlgRayBoxAABB::isCollided(vec3 boxMin, vec3 boxVolume, vec3 rayOrigin, vec3 rayDir) {

	vec3 boxMax = boxMin + boxVolume;

	float tnear, tfar, tymin, tymax, tzmin, tzmax;
	//check X. Set tnear and tfar to X coords(by default)
	if (rayDir.x >= 0) {
		tnear = (boxMin.x - rayOrigin.x) / rayDir.x;
		tfar = (boxMax.x - rayOrigin.x) / rayDir.x;
	}
	else {
		tnear = (boxMax.x - rayOrigin.x) / rayDir.x;
		tfar = (boxMin.x - rayOrigin.x) / rayDir.x;
	}
	//check Y
	if (rayDir.y >= 0) {
		tymin = (boxMin.y - rayOrigin.y) / rayDir.y;
		tymax = (boxMax.y - rayOrigin.y) / rayDir.y;
	}
	else {
		tymin = (boxMax.y - rayOrigin.y) / rayDir.y;
		tymax = (boxMin.y - rayOrigin.y) / rayDir.y;
	}
	if ((tnear > tymax) || (tymin > tfar)) return false;
	if (tymin > tnear) tnear = tymin;
	if (tymax < tfar) tfar = tymax;

	if (rayDir.z >= 0) {
		tzmin = (boxMin.z - rayOrigin.z) / rayDir.z;
		tzmax = (boxMax.z - rayOrigin.z) / rayDir.z;
	}
	else {
		tzmin = (boxMax.z - rayOrigin.z) / rayDir.z;
		tzmax = (boxMin.z - rayOrigin.z) / rayDir.z;
	}
	if ((tnear > tzmax) || (tzmin > tfar)) return false;
	if (tzmin > tnear) tnear = tzmin;
	if (tzmax < tfar) tfar = tzmax;

	this->tfar = tfar;
	this->tnear = tnear;

	return true;// ((tmin < t1) && (tmax > t0));

}

double AlgRayBoxAABB::getTNear() {
	return tnear;
}
double AlgRayBoxAABB::getTFar() {
	return tfar;
}
