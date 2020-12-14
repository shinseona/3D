#include "Object.h"
#include "Renderer.h"

Object::Object()
{
	translate = glm::translate(Renderer::instance()->ModelMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
}


void Object::SetPosition(glm::vec3 src_position)
{
	translate = glm::translate(Renderer::instance()->ModelMatrix, src_position);
}

void Object::SetRotation(float rot_angle, glm::vec3 axis)
{
	rotate = glm::rotate(Renderer::Renderer::instance()->ModelMatrix, glm::radians(rot_angle), axis);
}

void Object::SetScale(glm::vec3 scale_range)
{
	scale = glm::scale(Renderer::instance()->ModelMatrix, scale_range);
}
