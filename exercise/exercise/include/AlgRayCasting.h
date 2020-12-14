#pragma once
#ifndef _ALGRAYCAST_H_
#define _ALGRAYCAST_H_

#include <glm\vec3.hpp>
#include <glm\vec4.hpp>

using glm::vec3;
using glm::vec4;
using glm::inverse;
using namespace glm;

class AlgRayCasting {
private:
	vec3 origin;
	vec3 direction;
public:
	void calculate(float mouseX, float mouseY,
		float scrWidth, float scrHeight,
		mat4 projection, mat4 camViewMatrix);
	vec3 getOrigin();
	vec3 getDirection();
	float oX();
	float oY();
	float oZ();
	float dX();
	float dY();
	float dZ();
};

#endif


