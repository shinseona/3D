#pragma once

#include <vector>

#include "RenderableObject.h"

#include "include/GL/glew.h"		
#include "include/GLFW/glfw3.h" 

class RenderableObject;
class NonRenderableObject;
class IRenderer;
class IUpdater;

class Renderer : public ICleanUp
{
public : 
	Renderer();
	~Renderer() {}
	void Initialize();
	void draw();
	void AddObject(IRenderer* render_obj);
	void Update(IUpdater* src_obj);

	void SetViewMatrix(glm::mat4 src_ViewMatrix) {ViewMatrix = src_ViewMatrix; }
	GLFWwindow* GetWindow() const { return window; }

	virtual void ShutDown() override;

private : 
	glm::mat4 ModelMatrix;
	glm::mat4 ViewMatrix;
	glm::mat4 ProjectionMatrix;

	GLuint VertexArrayID;

	GLFWwindow* window;

	std::vector<IRenderer*> objectArray;
};