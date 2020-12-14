#include "Helper.h"
#include "Tile.h"
#include "GameWorld.h"
#include <glm/vec3.hpp>
#include <iostream>
#include<assimp/matrix4x4.h>
using std::cout;
using std::endl;
using glm::vec3;
using namespace std::chrono;



/*Normalize viewport coordinates*/
glm::vec2 Helper::toNormDevCoords(float viewportX, float viewportY, float scrWidth, float scrHeight) {
	float x, y;
	x = (2.0f * viewportX) / scrWidth - 1.0f;
	y = 1.0f - (2.0f * viewportY) / scrHeight;
	return glm::vec2(x, y);
}

/*Transform terrain coords to tile coord*/
int* Helper::toTileCoords(float terrainX, float terrainZ, float terrainMinX, float terrainMinZ) {
	int x, y;
	float offsetX = abs(terrainX - terrainMinX);
	float offsetZ = abs(terrainZ - terrainMinZ);
	x = (int)(offsetZ / 2);
	y = (int)(offsetX / 2);
	int* result = new int[2];
	result[0] = x;
	result[1] = y;
	return result;
}
/*Transform terrain coords to tile coord*/
vec2 Helper::toTileCoords(vec3 terrainCoords, vec3 terrainOrigin) {
	int x, y;
	vec3 offset = terrainCoords;// abs(terrainOrigin - terrainCoords);
	x = (int)(offset.z / 2);
	y = (int)(offset.x / 2);
	return vec2((int)x, (int)y);
}

/*Transform tile to coord*/
vec3 Helper::toWorldCoords(Tile t, vec3 terrainMin, vec3 terrainVolume) {
	float x, z;
	//x = terrainMin.x + t.getY() * 2 + 1;//^
	//z = (terrainMin.z + t.getX() * 2);//>
	terrainMin -= terrainVolume;
	x = terrainMin.x + t.getX() * 2+1;
	z = terrainMin.z + t.getY() * 2;
	return vec3(x, terrainMin.y, z);
}

/*Searching the value in interval [start, end] that is in interval with epsilon length*/
glm::vec3 Helper::binSearch(vec3 start, vec3 end, int currIteration, int iterations, float epsilon) {
	if (currIteration < iterations) {
		++currIteration;
		vec3 medium = (start + end) * 0.5f;
		if (medium.y == 0.0f)
			return medium;
		else if (medium.y > 0.0f)
			start = medium;
		else
			end = medium;
		if ((std::abs(end.y - start.y)) < epsilon) {
			return start;
		}
		return binSearch(start, end, currIteration, iterations, epsilon);
	}
	else
		return vec3(-1000.0f, 1000.0f, 1000.0f);
}

/*Get tiles that occupied by model*/
vector<Tile> Helper::getModelTiles(StaticModel m) {
	vector<Tile> tiles = getModelTiles(m.getCoords(), m.getVolume());
	return tiles;
}

vector<Tile> Helper::getModelTiles(vec3 coords, vec3 params) {
	/*
	^
	|
	x
	z---->
	*/
	vector<Tile> tiles;
	float x1 = coords.x, x2 = coords.x + params.x;
	float y1 = coords.z, y2 = coords.z + params.z;
	float restX, restY;

	//cout << "Params: " << params.x << ", " << params.y << ", " << params.z << endl;
	//cout << "Coords: " << coords.x << ", " << coords.y << ", " << coords.z << endl;
	GameWorld& g = GameWorld::getInstance();
	vec3 worldMin = g.getWorldOrigin();

	restX = fmod(x1, 2.0f);
	restY = fmod(y1, 2.0f);

	if (restX == 0) {
		if (restY == 0) {//x and y even number
			x1 += 1; 
			y1 += 1;
			for (float x = x1; x < x2; x += 2) {
				for (float y = y1; y < y2; y += 2) {
					vec2 tile = toTileCoords(vec3(x, 0, y), worldMin);
					tile.x -= 1;
					tile.y -= 1;
					tiles.push_back(tile);
				}
			}
		}
		else //only x even number
		{
			x1 += 1;
			for (float x = x1; x < x2; x += 2) {
				for (float y = y1; y <= y2; y += 2) {
					vec2 tile = toTileCoords(vec3(x, 0, y), worldMin);
					tile.x -= 1;
					tile.y -= 1;
					tiles.push_back(tile);
				}
			}
		}
	}
	else { 
		if (restY == 0) { //only y even number
			y1 += 1;
			for (float x = x1; x <= x2; x += 2) {
				for (float y = y1; y < y2; y += 2) {
					vec2 tile = toTileCoords(vec3(x, 0, y), worldMin);
					tile.x -= 1;
					tile.y -= 1;
					tiles.push_back(tile);
				}
			}
		}
		else //both x1 and y1 odd number
		{
			for (float x = x1; x <= x2; x += 2) {
				for (float y = y1; y <= y2; y += 2) {
					vec2 tile = toTileCoords(vec3(x, 0, y), worldMin);
					tile.x -= 1;
					tile.y -= 1;
					tiles.push_back(tile);
				}
			}
		}
	}

	return tiles;
}


glm::mat4 Helper::aiMatrix4x4ToGlm(const aiMatrix4x4 &from)
{
	glm::mat4 to;
	//the a,b,c,d in assimp is the row
	//the 1,2,3,4 is the column
	/*to[0][0] = from.a1; to[0][1] = from.a2; to[0][2] = from.a3; to[0][3] = from.a4;
	to[1][0] = from.b1; to[1][1] = from.b2; to[1][2] = from.b3; to[1][3] = from.b4;
	to[2][0] = from.c1; to[2][1] = from.c2; to[2][2] = from.c3; to[2][3] = from.c4;
	to[3][0] = from.d1; to[3][1] = from.d2; to[3][2] = from.d3; to[3][3] = from.d4;
*/
	to[0][0] = from.a1; to[1][0] = from.a2;
	to[2][0] = from.a3; to[3][0] = from.a4;
	to[0][1] = from.b1; to[1][1] = from.b2;
	to[2][1] = from.b3; to[3][1] = from.b4;
	to[0][2] = from.c1; to[1][2] = from.c2;
	to[2][2] = from.c3; to[3][2] = from.c4;
	to[0][3] = from.d1; to[1][3] = from.d2;
	to[2][3] = from.d3; to[3][3] = from.d4;

	return to;
}

glm::vec3 Helper::assimpVec3ToGlmVec3(aiVector3D assimpVec) {
	glm::vec3 glmVec;

	glmVec.x = assimpVec.x;
	glmVec.y = assimpVec.y;
	glmVec.z = assimpVec.z;

	return glmVec;
}

glm::quat Helper::assimpQuatToGlmQuat(aiQuaternion assimpQuat) {
	glm::quat glmQuat;
	glmQuat.x = assimpQuat.x;
	glmQuat.y = assimpQuat.y;
	glmQuat.z = assimpQuat.z;
	glmQuat.w = assimpQuat.w;
	return glmQuat;
}

int Helper::max(int v1, int v2, int v3) {
	int value = 0;
	if (v1 > v2) {
		value = v1;
		if (value < v3) value = v3;
	}
	else {
		value = v2;
		if (value < v3) value = v3;
	}
	return value;
}


void Helper::showMatrix(mat4 matrix) {
	for (int a = 0; a < 4; ++a) {
		for (int b = 0; b < 4; ++b) {
			//printf("%.7f ", matrix[a][b]);
			cout << matrix[a][b] << " ";
			assert(!isnan(matrix[a][b]));
		}
		cout << endl;
	}
}

aiMatrix4x4 Helper::glmToAssimpMat4x4(mat4 input) {
	aiMatrix4x4 output;

	//the a,b,c,d in assimp is the row
	//the 1,2,3,4 is the column
	output.a1 = input[0][0]; output.a2 = input[0][1]; output.a3 = input[0][2]; output.a4 = input[0][3];
	output.b1 = input[1][0]; output.b2 = input[1][1]; output.b3 = input[1][2]; output.b4 = input[1][3];
	output.c1 = input[2][0]; output.c2 = input[2][1]; output.c3 = input[2][2]; output.c4 = input[2][3];
	output.d1 = input[3][0]; output.d2 = input[3][1]; output.d3 = input[3][2]; output.d4 = input[3][3];
	return output;
}

void Helper::showVector(vec3 vector) {
	cout << vector.x << " " << vector.y << " " << vector.z << " " << endl;
}
void Helper::showVector(vec4 vector) {
	cout << vector.x << " " << vector.y << " " << vector.z << " " << vector.w << endl;
}
void Helper::showVector(vec2 vector) {
	cout << vector.x << " " << vector.y << endl;
}
void Helper::showVector(ivec4 vector) {
	cout << vector.x << " " << vector.y << " " << vector.z << " " << vector.w<< endl;
}
void Helper::showVector(aiVector3D vector) {
	cout << vector.x << " " << vector.y << " " << vector.z << endl;
}
void Helper::showQuat(quat q) {
	printf("%.6f %.6f %.6f %.6f\n", q.x, q.y, q.z, q.w);
}

timePoint Helper::getCurrentTimePoint() {
	return std::chrono::system_clock::now();
}
long long Helper::getDeltaTime(timePoint start, timePoint end) {
	long long delta = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
	return delta;
}
long long Helper::getDeltaTime(long long start, long long end) {
	return end - start;
}

long long Helper::getCurrentTime() {
	milliseconds ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
	long long currTime = ms.count();
	return currTime;
}

float Helper::calcDegreeBetween(vec3 v1, vec3 v2) {
	float len1 = length(v1);
	float len2 = length(v2);
	float product = dot(v1, v2);
//	return acos(product / (len1*len2));
	return degrees(acos(product / (len1*len2)));
//	return (product / (len1*len2)*180.0f) / 3.1459;
}

float Helper::calcDegreeBetween(vec2 v1, vec2 v2) {
	float len1 = length(v1);
	float len2 = length(v2);
	float product = dot(v1, v2);
	return acos(product / (len1*len2));
}

unsigned int Helper::textureFromFile(const char *path, bool gamma)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;

	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);

	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

unsigned int Helper::textureFromFile(string path, bool gamma) {
	return textureFromFile(path.c_str(), gamma);
}

void Helper::flip(double& v1, double& v2) {
	double tmp = v1;
	v1 = v2;
	v2 = tmp;
}
void Helper::flip(float& v1, float& v2) {
	float tmp = v1;
	v1 = v2;
	v2 = tmp;
}


