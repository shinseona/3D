#ifndef __RENDERABLEOBJECT_H__
#define __RENDERABLEOBJECT_H__

#include "Object.h"

class Object;
class FileManager;

class RenderableObject : public Object
{
public:
	RenderableObject() {}
	~RenderableObject(){}

	void SetBuffer();

	virtual void ShutDown() override;

public:
	GLuint vertexbuffer;
	GLuint uvbuffer;
	GLuint normalbuffer;

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;

	GLuint programID;
	GLuint matrixID;
	GLuint viewMatrixID;
	GLuint modelMatrixID;

	glm::mat4 ProjectionMatrix;
	glm::mat4 ViewMatrix;
	glm::mat4 ModelMatrix;


	GLuint texture;
	GLuint textureID;
	
};


#endif // !__OBJECT_H__#pragma once
