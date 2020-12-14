#pragma once
#ifndef ALGBOXBOXABB_H_
#define ALGBOXBOXABB_H_

#include <glm/vec3.hpp>
using glm::vec3;

class AlgBoxBoxAABB {
private:

public:
	bool checkCollide(vec3 box1Min, vec3 box1Max, vec3 box2Min, vec3 box2Max);
};

#endif
