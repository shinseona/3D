#ifndef RENDEROBJECT_H
#define RENDEROBJECT_H

#include "Object.h"

class Object;
class FileManager;

class RenderObject : public Object
{
public:
	RenderObject() {}
	~RenderObject() {}

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
	GLuint LightRotID;
	GLuint modelMatrixID;
	GLuint VertexArrayID;
	GLuint LightID;
	GLuint uvoffsetID;

	glm::mat4 ProjectionMatrix;
	glm::mat4 ViewMatrix;
	glm::mat4 ModelMatrix;
	glm::vec3 lightPos;

	GLuint texture;
	GLuint textureID;


	virtual void Init();


};
#endif // RENDEROBJECT_H
