#pragma once

#include "include/GL/glew.h"		
#include "include/GLFW/glfw3.h" 
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

class Controller
{
public:
	static Controller* instance()
	{
		static Controller instance;

		return &instance;
	}

	void MatrixInput();

	glm::mat4 ViewMatrix;
	glm::mat4 ProjectionMatrix;

	glm::mat4 getViewMatrix() 
	{
		return ViewMatrix;
	}

	glm::mat4 getProjectionMatrix()
	{
		return ProjectionMatrix;
	}

	glm::vec3 position = glm::vec3(0, 0, 5);

	float horizontalAngle = 3.14f;

	float verticalAngle = 0.0f;

	float initialFoV = 45.0f;
	float speed = 3.0f;
	float mouseSpeed = 0.00001f;
};