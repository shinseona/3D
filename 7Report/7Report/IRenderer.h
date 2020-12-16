#ifndef __IRENDERER_H__
#define __IRENDERER_H__

class IRenderer
{
public :
	virtual void InitData() = 0;
	virtual glm::mat4 GetTranslate() = 0;

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

#endif // !__IRENDERER_H__

