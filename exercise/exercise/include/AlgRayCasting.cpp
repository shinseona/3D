//made by Romeon0
#include <glm\vec3.hpp>
#include <glm\vec4.hpp>
#include <glm\mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "AlgRayCasting.h"
#include <iostream>
using glm::vec3;
using glm::vec4;
using glm::inverse;
using namespace glm;


void AlgRayCasting::calculate(float mouseX, float mouseY, float scrWidth, float scrHeight, mat4 projection, mat4 camViewMatrix) {
	vec4 viewport = vec4(0.0f, 0.0f, scrWidth, scrHeight);
	vec3 screenPos = vec3(mouseX, scrHeight - mouseY - 1, 0.0f);

	origin = unProject(screenPos, camViewMatrix, projection, viewport);//near
	screenPos = vec3(mouseX, scrHeight - mouseY - 1, 1.0f);
	vec3 destiny = unProject(screenPos, camViewMatrix, projection, viewport);//far
	direction = vec3(destiny.x - origin.x, destiny.y - origin.y, destiny.z - origin.z);
}

vec3 AlgRayCasting::getOrigin() { return origin; }
vec3 AlgRayCasting::getDirection() { return direction; }
float AlgRayCasting::oX() { return origin.x; }
float AlgRayCasting::oY() { return origin.y; }
float AlgRayCasting::oZ() { return origin.z; }
float AlgRayCasting::dX() { return direction.x; }
float AlgRayCasting::dY() { return direction.y; }
float AlgRayCasting::dZ() { return direction.z; }

