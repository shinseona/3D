//#pragma comment(lib, "glew32s.lib")
#include <GLEW/glew.h>
#include <GLFW/glfw3.h>
#include <gl/GL.h> // holds all OpenGL type declarations

#include <assimp/Importer.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"
#include "BoneMesh.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include<glm/vec4.hpp>
#include "Helper.h"

using glm::vec4;
using namespace std;

/*  Functions  */
// constructor
BoneMesh::BoneMesh(vector<BoneVertex>& vertices, vector<unsigned int>& indices, vector<Texture>& textures){
	cout << "BoneMesh::BoneMesh(v,i,t)\n";
	assert(vertices.size() != 0);
	assert(indices.size() != 0);

	this->vertices.insert(this->vertices.end(), vertices.begin(), vertices.end());
	this->indices.insert(this->indices.end(), indices.begin(), indices.end());
	this->textures.insert(this->textures.end(), textures.begin(), textures.end());

	calculateVolume();
	setupMesh();
}
BoneMesh::BoneMesh(const BoneMesh& boneMesh) {
	cout << "BoneMesh::BoneMesh(boneMesh)\n";
	system("pause");
	assert(boneMesh.vertices.size() != 0);
	assert(boneMesh.indices.size() != 0);

	this->vertices.insert(this->vertices.end(), boneMesh.vertices.begin(), boneMesh.vertices.end());
	this->indices.insert(this->indices.end(), boneMesh.indices.begin(), boneMesh.indices.end());
	this->textures.insert(this->textures.end(), boneMesh.textures.begin(), boneMesh.textures.end());
	this->VAO = boneMesh.VAO;
	this->VBO = boneMesh.VBO;
	this->EBO = boneMesh.EBO;
	this->width = boneMesh.width;
	this->height = boneMesh.height;
	this->depth = boneMesh.depth;
	this->x = boneMesh.x;
	this->y = boneMesh.y;
	this->z = boneMesh.z;
}
BoneMesh::BoneMesh() {
	this->vertices = vector<BoneVertex>();
}

BoneMesh::~BoneMesh(){
	cout << "BoneMesh::~BoneMesh1\n";
	cout << "BoneMesh::~BoneMesh2\n";
	glBindVertexArray(0);
	glDeleteBuffers(1, &EBO);
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);
	cout << "BoneMesh::~BoneMesh3\n";
}

BoneMesh & BoneMesh::operator=(const BoneMesh & b){
	cout << "Called BoneMesh::operator=1\n";
	color = b.color;
	colorMode = b.colorMode;
	depth = b.depth;
	width = b.width;
	height = b.height;
	x = b.x;
	y = b.y;
	z = b.z;
	for (int a = 0; a < b.indices.size(); ++a) {
		indices.push_back(b.indices.at(a));
	}
	for (int a = 0; a < b.vertices.size(); ++a) {
		vertices.push_back(b.vertices.at(a));
	}
	for (int a = 0; a < b.textures.size(); ++a) {
		textures.push_back(b.textures.at(a));
	}
	VAO = b.VAO;
	VBO = b.VBO;
	EBO = b.EBO;
	return *this;
}

BoneMesh* BoneMesh::operator=(const BoneMesh* b) {
	cout << "Called BoneMesh::operator=2\n";
	color = b->color;
	colorMode = b->colorMode;
	depth = b->depth;
	width = b->width;
	height = b->height;
	x = b->x;
	y = b->y;
	z = b->z;
	for (int a = 0; a < b->indices.size(); ++a) {
		indices.push_back(b->indices.at(a));
	}
	for (int a = 0; a < b->vertices.size(); ++a) {
		vertices.push_back(b->vertices.at(a));
	}
	for (int a = 0; a < b->textures.size(); ++a) {
		textures.push_back(b->textures.at(a));
	}
	VAO = b->VAO;
	VBO = b->VBO;
	EBO = b->EBO;
	return this;
}

vector<BoneVertex> BoneMesh::getVertices() {
	return vertices;
}
vector<unsigned int> BoneMesh::getIndices() {
	return indices;
}

void BoneMesh::calculateVolume() {
	glm::vec3 min(1000.0, 1000.0, 1000.0);
	glm::vec3 max(0.0, 0.0, 0.0);
	glm::vec3 pos;
	for (int a = 0; a < vertices.size(); ++a) {
		pos = vertices[a].position;
		if (pos.x < min.x)
			min.x = pos.x;
		if (pos.y < min.y)
			min.y = pos.y;
		if (pos.z < min.z)
			min.z = pos.z;

		if (pos.x > max.x)
			max.x = pos.x;
		if (pos.y > max.y)
			max.y = pos.y;
		if (pos.z > max.z)
			max.z = pos.z;
	}
	width = abs(max.x - min.x);
	height = abs(max.y - min.y);
	depth = abs(max.z - min.z);
	x = -min.x;
	y = -min.y;
	z = -min.z;
}
// render the mesh
void BoneMesh::draw(Shader& shader) {
	if (vertices.size() == 0) return;

	// bind textures
	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;
	unsigned int normalNr = 1;
	unsigned int heightNr = 1;
	for (unsigned int i = 0; i < textures.size(); i++){
		glActiveTexture(GL_TEXTURE0 + i); 
										  
		stringstream ss;
		string number;
		string name = textures[i].type;
		if (name == "texture_diffuse")
			ss << diffuseNr++; 
		else if (name == "texture_specular")
			ss << specularNr++; 
		else if (name == "texture_normal")
			ss << normalNr++; 
		else if (name == "texture_height")
			ss << heightNr++; 
		number = ss.str();
		glUniform1i(glGetUniformLocation(shader.getId(), (name + number).c_str()), i);
		glBindTexture(GL_TEXTURE_2D, textures[i].id);
	}

	// draw mesh
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, (void*)0);

	//set to default
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(0);
}

glm::vec3 BoneMesh::getVolume() {
	return glm::vec3(width, height, depth);
}
glm::vec3 BoneMesh::getCoords() {
	return glm::vec3(x, y, z);
}

/*  Functions    */
// initializes all the buffer objects/arrays
void BoneMesh::setupMesh()
{
	// create buffers/arrays
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glBindVertexArray(VAO);
	// load data into vertex buffers
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(BoneVertex), &vertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);


	// set the vertex attribute pointers
	// vertex Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(BoneVertex), (void*)0);
	// vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(BoneVertex), (void*)offsetof(BoneVertex, normal));
	// vertex texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(BoneVertex), (void*)offsetof(BoneVertex, texCoords));
	// bone ids
	glEnableVertexAttribArray(3);
	glVertexAttribIPointer(3, 4, GL_INT, sizeof(BoneVertex), (void*)offsetof(BoneVertex, boneIds));
	// bone weights
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(BoneVertex), (void*)offsetof(BoneVertex, boneWeights));
	
	glBindVertexArray(0);
}

void BoneMesh::addBoneData(unsigned int vertexId, float weight, unsigned int boneId) {
	//printf("Im here: %d% , %.7f :: %d \n", vertexId, weight, vertices.size());

	vec4 weights = vertices.at(vertexId).boneWeights;
	bool found = false;
	for (int a = 0; a < 4; ++a) {
		if (weights[a] == 0.0f) {
			vertices.at(vertexId).boneWeights[a] = weight;
			vertices.at(vertexId).boneIds[a] = boneId;
			found = true;
			break;
		}
	}
	assert(found);//more than 4 bones influences this vertex
}

void BoneMesh::update() {
	if (vertices.size() == 0) return;

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(BoneVertex), &vertices[0], GL_STATIC_DRAW);
	glBindVertexArray(0);
}