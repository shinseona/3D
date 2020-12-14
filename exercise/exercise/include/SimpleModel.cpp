
#pragma comment(lib, "glew32s.lib")
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "mesh.h"
#include "shader.h"
#include "Helper.h"
#include <string>
#include <iostream>
#include <vector>
#include "SimpleModel.h"
#include <gl/GL.h>
using namespace std;
using namespace glm;

SimpleModel::SimpleModel(vector<Vertex> vertices, vector<unsigned int> indices, string texturePath) {
//	this->vertices = vertices;
//	this->indices = indices;
	

	if (this->vertices.size() == 0) {
		Vertex v1, v2, v3;
		v1.Position = vec3(0.0, 0.0, 0.0);
		v2.Position = vec3(0.0, 0.0, 0.0);
		v3.Position = vec3(0.0, 0.0, 0.0);
		this->vertices.push_back(v1);
		this->vertices.push_back(v2);
		this->vertices.push_back(v3);
	}
	if (this->indices.size() == 0) {
		this->indices.push_back(0);
		this->indices.push_back(1);
		this->indices.push_back(2);
	}

	initModel(texturePath);

	updateIndices(indices);
	updateVertices(vertices);
}
SimpleModel::~SimpleModel() {
	indices.clear();
	vertices.clear();
	glBindVertexArray(0);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ibo);
	glDeleteVertexArrays(1, &vao);
}

vector<Vertex> SimpleModel::getVertices() { return vertices; }
vector<unsigned int> SimpleModel::getIndices() { return indices; }
void SimpleModel::setName(string n) { name = n; }

void SimpleModel::draw(Shader& shader, GLenum drawMode) {
	if (vertices.size() == 0) { cout << "NO VERTICES!\n"; return; }
	
	/*glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*vertices.size(), &vertices[0], GL_DYNAMIC_DRAW);
	glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);
	//glDrawArrays(drawMode, 0, (GLsizei)vertices.size());
	glBindVertexArray(0);*/

	//set texture, is exists
	if (textureId != -1) {
		int i = 0;
		glActiveTexture(GL_TEXTURE0 + i);
		glUniform1i(glGetUniformLocation(shader.getId(), "texture_diffuse1"), i);
		glBindTexture(GL_TEXTURE_2D, textureId);
	}

	//update vertices
	//glBindVertexArray(vao);
	//glBindBuffer(GL_ARRAY_BUFFER, vbo);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*vertices.size(), &vertices[0], GL_DYNAMIC_DRAW);
	
	//draw
	glBindVertexArray(vao);
	glDrawElements(drawMode, (GLsizei)indices.size(), GL_UNSIGNED_INT, (void*)0);
	//glDrawArrays(drawMode, 0, (GLsizei)vertices.size());

	//set to default
	glBindVertexArray(0);
	glActiveTexture(GL_TEXTURE0);
}

void SimpleModel::translate(float x, float y, float z) {
	matrix = glm::translate(matrix, glm::vec3(x, y, z));
	this->x -= x;
	this->y -= y;
	this->z -= z;
}
void SimpleModel::scale(float x, float y, float z) {/*TODO implement function*/}
glm::mat4 SimpleModel::getMatrix() { return matrix; }
float SimpleModel::getWidth() { return width; }
float SimpleModel::getHeight() { return height; }
float SimpleModel::getDepth() { return depth; }
glm::vec3 SimpleModel::getVolume() { return vec3(width, height, depth); }
glm::vec3 SimpleModel::getCoords() { return vec3(x, y, z); }
void SimpleModel::setCoords(glm::vec3 coord) { 
	x = coord.x;
	y = coord.y;
	z = coord.z;
}

void SimpleModel::initModel(string texturePath) {
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ibo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	//vertex coord
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	// vertex texture coords
	if (texturePath.length() != 0) {
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
		int i = 0;
		Helper h;
		textureId = h.textureFromFile(texturePath, false);
	}

	glBindVertexArray(0);
	glActiveTexture(GL_TEXTURE0);
}

void SimpleModel::updateIndices(vector<unsigned int> newIndices) {
	if (newIndices.size() == 0) return;
	indices = newIndices;
	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
}
void SimpleModel::updateVertices(vector<Vertex> newVertices) {
	if (newVertices.size() == 0) return;
	vertices = newVertices;
	
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
}