#include "NonRenderableObject.h"
#include "Renderer.h"

#include "include/GL/glew.h"
#include "include/GLFW/glfw3.h" 
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

NonRenderableObject::NonRenderableObject()
{
	Renderer* renderer = Renderer::instance();
	renderer->addNonRenderableObject(this);
}

void NonRenderableObject::init()
{

}

void NonRenderableObject::render()
{

}

void NonRenderableObject::update()
{

}

void NonRenderableObject::shutDown()
{

}