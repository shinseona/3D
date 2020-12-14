#include "AlgBoxBoxAABB.h"


bool AlgBoxBoxAABB::checkCollide(vec3 box1Min, vec3 box1Max, vec3 box2Min, vec3 box2Max) {
	return (box1Min.x <= box2Max.x && box1Max.x >= box2Min.x)
		&& (box1Min.y <= box2Max.y && box1Max.y >= box2Min.y)
		&& (box1Min.z <= box2Max.z && box1Max.z >= box2Min.z);
}
