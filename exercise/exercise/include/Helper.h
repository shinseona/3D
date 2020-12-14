#pragma once
#ifndef HELPER_H_
#define HELPER_H_
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include <assimp/quaternion.h>
#include <cmath>
#include "Tile.h"
#include "StaticModel.h"
#include <vector>
#include <chrono>
using namespace std::chrono;

typedef time_point<system_clock> timePoint;


using glm::vec3;
using glm::vec2;
using glm::quat;

class Helper {
private:
public:
	/*Normalize viewport coordinates*/
	glm::vec2 toNormDevCoords(float viewportX, float viewportY, float scrWidth, float scrHeight);

	/*Transform terrain coords to tile coord*/
	int* toTileCoords(float terrainX, float terrainZ, float terrainMinX, float terrainMinZ);
	/*Transform terrain coords to tile coord*/
	vec2 toTileCoords(vec3 terrainCoords, vec3 terrainMin);

	/*Transform tile to coord*/
	vec3 toWorldCoords(Tile t, vec3 terrainMin, vec3 terrainVolume);

	/*Searching the value in interval [start, end] that is in interval with epsilon length*/
	glm::vec3 binSearch(vec3 start, vec3 end, int currIteration, int iterations, float epsilon);

	vector<Tile> getModelTiles(StaticModel m);
	vector<Tile> Helper::getModelTiles(vec3 params, vec3 coords);
	glm::vec3 assimpVec3ToGlmVec3(aiVector3D assimpVec);
	glm::quat assimpQuatToGlmQuat(aiQuaternion assimpQuat);
	glm::mat4 Helper::aiMatrix4x4ToGlm(const aiMatrix4x4 &from);
	int max(int v1, int v2, int v3);

	void showMatrix(mat4 matrix);
	void showVector(vec3 vector);
	void showVector(vec4 vector);
	void showVector(ivec4 vector);
	void showVector(vec2 vector);
	void showVector(aiVector3D vector);
	void showQuat(quat q);

	timePoint getCurrentTimePoint();
	long long getDeltaTime(timePoint start, timePoint end);
	long long getDeltaTime(long long start, long long end);
	long long getCurrentTime();
	float calcDegreeBetween(vec2 v1, vec2 v2);
	float calcDegreeBetween(vec3 v1, vec3 v2);
	unsigned int textureFromFile(const char *path, bool gamma=false);
	unsigned int textureFromFile(string path, bool gamma=false);
	void flip(double& v1, double& v2);
	void flip(float& v1, float& v2);
	aiMatrix4x4 glmToAssimpMat4x4(mat4 input);

};


#endif