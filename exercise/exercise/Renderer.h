#ifndef RENDERER_H
#define RENDERER_H


#include <vector>

#include "RenderObject.h"
#include "IUpdate.h"

#include "include/GL/glew.h"		
#include "include/GLFW/glfw3.h" 

#include <Windows.h>
#include <MMSystem.h>

class RenderObject;

class Renderer : public ICleanUp,public IUpdate
{
public:

	static Renderer* instance() {

		static Renderer instance;

		return &instance;
	}
	
	Renderer();
	~Renderer() {}
	void Init();
	void draw(RenderObject* object);
	void Rotationdraw(RenderObject* object);

	void SetViewMatrix(glm::mat4 src_ViewMatrix) {ViewMatrix = src_ViewMatrix; }
	void SetProjectionMatrix(glm::mat4 src_ViewMatrix) { ProjectionMatrix = src_ViewMatrix; }

	GLFWwindow* GetWindow() const { return window; }

public:
	void Update(IUpdate* iupdate);
	virtual void ShutDown() override;

	float uv_offset = 0.0f;

	glm::mat4 ModelMatrix;
	glm::mat4 ViewMatrix;
	glm::mat4 ProjectionMatrix;
	glm::mat4 LightRot;

	bool isRenderTiming();

	GLuint VertexArrayID;

	GLFWwindow* window;

	LARGE_INTEGER _currentFrameCounter;
	LARGE_INTEGER _prevFrameCounter;
	LARGE_INTEGER _frameInfo;

	double _perFrame;
};
#endif // !RENDERER_H
