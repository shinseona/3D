#pragma once

#include "Object.h"

class RenderableObject : public Object
{
public:
	glm::vec3 ObjPosition;
	glm::vec3 Objscale;

	GLuint VertexArrayID;
	GLuint programID;
	GLuint MatrixID;
	GLuint ViewMatrixID;
	GLuint ModelMatrixID;
	GLuint Texture;
	GLuint TextureID;

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;

	GLuint vertexbuffer;
	GLuint uvbuffer;
	GLuint normalbuffer;
	GLuint LightID;

public:
	RenderableObject();
	virtual void init() {};
	virtual void render() override {};
	virtual void update() override {};
	virtual void shutDown() override {};
	virtual void setPosition(float x, float y, float z) {};
	virtual void setScale(float x, float y, float z) {};
};