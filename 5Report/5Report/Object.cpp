#include "Object.h"

Object::Object()
{
	translate = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
}

void Object::SetPosition(glm::vec3 src_position)
{
	translate = glm::translate(glm::mat4(1.0f), src_position);
}

void Object::ShutDown()
{

}