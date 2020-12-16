#include <stdio.h>

#include "Object.h"
#include "Renderer.h"
#include "RenderableObject.h"
#include "NonRenderableObject.h"

#include "include/GL/glew.h"
#include "include/GLFW/glfw3.h" 
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

void Renderer::init()
{
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(1024, 768, "3D", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		getchar();
		glfwTerminate();
		return;
	}
	glfwMakeContextCurrent(window);

	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return;
	}

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Set the mouse at the center of the screen

	glfwSetCursorPos(window, 1024 / 2, 768 / 2);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_CULL_FACE);

	for (std::vector<RenderableObject*>::iterator it = renderList.begin(); it != renderList.end(); it++)
	{
		(*it)->init();
	}

	for (std::vector<NonRenderableObject*>::iterator it = nonrenderList.begin(); it != nonrenderList.end(); it++)
	{
		(*it)->init();
	}

}

void Renderer::render()
{
	for (std::vector<RenderableObject*>::iterator it = renderList.begin(); it != renderList.end(); it++)
	{
		(*it)->render();
	}

	for (std::vector<NonRenderableObject*>::iterator it = nonrenderList.begin(); it != nonrenderList.end(); it++)
	{
		(*it)->render();
	}
}

void Renderer::update()
{
	for (std::vector<RenderableObject*>::iterator it = renderList.begin(); it != renderList.end(); it++)
	{
		(*it)->update();
	}

	for (std::vector<NonRenderableObject*>::iterator it = nonrenderList.begin(); it != nonrenderList.end(); it++)
	{
		(*it)->update();
	}
}

void Renderer::shutDown()
{
	for (std::vector<RenderableObject*>::iterator it = renderList.begin(); it != renderList.end(); it++)
	{
		(*it)->shutDown();
	}

	for (std::vector<NonRenderableObject*>::iterator it = nonrenderList.begin(); it != nonrenderList.end(); it++)
	{
		(*it)->shutDown();
	}

	glfwTerminate();
}

void Renderer::addRenderableObject(RenderableObject* src_obj)
{
	renderList.push_back(src_obj);
}

void Renderer::addNonRenderableObject(NonRenderableObject* src_obj)
{
	nonrenderList.push_back(src_obj);
}

void Renderer::Clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::Exit()
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		exit(0);
	}
}