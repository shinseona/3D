#pragma once
#ifndef MESH_H
#define MESH_H
#include "shader.h"
#include <vector>
#include <glm\glm.hpp>
#include <assimp\Importer.hpp>
using namespace std;

struct Vertex {
	// position
	glm::vec3 Position;
	// normal
	glm::vec3 Normal;
	// texCoords
	glm::vec2 TexCoords;
	// tangent
	glm::vec3 Tangent;
	// bitangent
	glm::vec3 Bitangent;
};

struct Texture {
	unsigned int id;
	string type;
	string path;
};

class Mesh {
public:
	/*  Mesh Data  */
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<Texture> textures;
	unsigned int VAO, VBO, EBO;

	glm::vec3 color;//for click detection
	bool colorMode = false;
	float width, height, depth;
	float x, y, z;

	/*  Functions  */
	// constructor
	Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures);
	~Mesh();

	void calculateVolume();
	// render the mesh
	void draw(Shader* shader);
	//getters
	glm::vec3 getVolume();
	glm::vec3 getCoords();
	vector<Vertex> getVertices();
	vector<unsigned int> getIndices();


private:
	/*  Render data  */

	/*  Functions    */
	// initializes all the buffer objects/arrays
	void setupMesh();
	void setupMesh2();
};
#endif