#pragma once

#include <vector>

#include "RenderableObject.h"

#include "include/GL/glew.h"		
#include "include/GLFW/glfw3.h" 

class RenderableObject;

class Renderer : public ICleanUp
{
public : 
	Renderer();
	~Renderer() {}
	void Initialize();
	void draw(RenderableObject* object);

	void SetViewMatrix(glm::mat4 src_ViewMatrix) {ViewMatrix = src_ViewMatrix; }
	GLFWwindow* GetWindow() const { return window; }

	virtual void ShutDown() override;

private : 
	glm::mat4 ModelMatrix;
	glm::mat4 ViewMatrix;
	glm::mat4 ProjectionMatrix;

	GLuint VertexArrayID;

	GLFWwindow* window;
};