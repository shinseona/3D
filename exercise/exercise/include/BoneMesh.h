#pragma once
#ifndef BONEMESH_H
#define BONEMESH_H
#include <vector>
#include <glm\glm.hpp>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <assimp\Importer.hpp>
#include "shader.h"
#include "mesh.h"
#include "Shader.h"
#include "Mesh.h"
using std::vector;
using glm::vec3;
using glm::vec4;
using glm::ivec4;
using glm::vec2;
using std::string;

struct BoneVertex {
	vec3 position = vec3(0, 0, 0);
	vec3 normal = vec3(0, 0, 0);
	vec2 texCoords = vec2(0, 0);
	ivec4 boneIds = ivec4(0, 0, 0, 0);
	vec4 boneWeights = vec4(0.0f, 0.0f, 0.0f, 0.0f);
};

class BoneMesh {
private:
	void setupMesh();
public:
	vector<BoneVertex> vertices;
	vector<unsigned int> indices;
	vector<Texture> textures;
	unsigned int VAO, VBO, EBO;

	glm::vec3 color;//for click detection
	bool colorMode = false;
	float width, height, depth;
	float x, y, z;

	// constructor
	BoneMesh(vector<BoneVertex>& vertices, vector<unsigned int>& indices, vector<Texture>& textures);
	BoneMesh(const BoneMesh& boneMesh);
	BoneMesh();
	~BoneMesh();
	BoneMesh& operator=(const BoneMesh& b);
	BoneMesh* operator=(const BoneMesh* b);

	void calculateVolume();
	// render the mesh
	void draw(Shader& shader);
	//getters
	glm::vec3 getVolume();
	glm::vec3 getCoords();
	vector<BoneVertex> getVertices();
	vector<unsigned int> getIndices();
	void addBoneData( unsigned int vertexId, float weight, unsigned int boneId);

	void update();

};
#endif