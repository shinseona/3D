#include "RenderableObject.h"
#include "Renderer.h"

#include "include/GL/glew.h"
#include "include/GLFW/glfw3.h" 
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

RenderableObject::RenderableObject()
{
	Renderer* renderer = Renderer::instance();
	renderer->addRenderableObject(this);
}