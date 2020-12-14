#include "RenderObject.h"
#include "FileManager.h"

void RenderObject::Init()
{
	textureID = glGetUniformLocation(programID, "myTextureSampler");

	programID = FileManager::instance()->LoadShaders("20171189_vs.shader", "20171189_fs.shader");
}
void RenderObject::SetBuffer()
{
	matrixID = glGetUniformLocation(programID, "MVP");
	viewMatrixID = glGetUniformLocation(programID, "V");
	modelMatrixID = glGetUniformLocation(programID, "M");
	 LightRotID = glGetUniformLocation(programID, "LightRot");

	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);

	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
	
	LightID = glGetUniformLocation(programID, "LightPosition_worldspace");

	uvoffsetID = glGetUniformLocation(programID, "uvoffset");
}
void RenderObject::ShutDown()
{
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &uvbuffer);
	glDeleteBuffers(1, &normalbuffer);
	glDeleteProgram(programID);
}